/*
 * Copyright (c) 2006,2011-2014 Apple Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

//
// reqinterp - Requirement language (exprOp) interpreter
//

#include "reqinterp.h"
#include "codesigning_dtrace.h"
#include <Security/SecTrustSettingsPriv.h>
#include <Security/SecCertificatePriv.h>
#include <security_utilities/memutils.h>
#include <security_utilities/logging.h>
#include <sys/csr.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFUnserialize.h>
#include "csutilities.h"
#include "notarization.h"

namespace Security {
namespace CodeSigning {


//
// Fragment fetching, caching, and evaluation.
//
// Several language elements allow "calling" of separate requirement programs
// stored on disk as (binary) requirement blobs. The Fragments class takes care
// of finding, loading, caching, and evaluating them.
//
// This is a singleton for (process global) caching. It works fine as multiple instances,
// at a loss of caching effectiveness.
//
class Fragments {
public:
	Fragments();
	
	bool named(const std::string &name, const Requirement::Context &ctx)
		{ return evalNamed("subreq", name, ctx); }
	bool namedAnchor(const std::string &name, const Requirement::Context &ctx)
		{ return evalNamed("anchorreq", name, ctx); }

private:
	bool evalNamed(const char *type, const std::string &name, const Requirement::Context &ctx);
	CFDataRef fragment(const char *type, const std::string &name);
	
	typedef std::map<std::string, CFRef<CFDataRef> > FragMap;
	
private:
	CFBundleRef mMyBundle;			// Security.framework bundle
	Mutex mLock;					// lock for all of the below...
	FragMap mFragments;				// cached fragments
};

static ModuleNexus<Fragments> fragments;


//
// Magic certificate features
//
static CFStringRef appleIntermediateCN = CFSTR("Apple Code Signing Certification Authority");
static CFStringRef appleIntermediateO = CFSTR("Apple Inc.");


//
// Main interpreter function.
//
// ExprOp code is in Polish Notation (operator followed by operands),
// and this engine uses opportunistic evaluation.
//
bool Requirement::Interpreter::evaluate()
{ return eval(stackLimit); }

bool Requirement::Interpreter::eval(int depth)
{
	if (--depth <= 0)		// nested too deeply - protect the stack
		MacOSError::throwMe(errSecCSReqInvalid);
	
	ExprOp op = ExprOp(get<uint32_t>());
	CODESIGN_EVAL_REQINT_OP(op, this->pc() - sizeof(uint32_t));
	switch (op & ~opFlagMask) {
	case opFalse:
		return false;
	case opTrue:
		return true;
	case opIdent:
		return mContext->directory && getString() == mContext->directory->identifier();
	case opAppleAnchor:
		return appleSigned();
	case opAppleGenericAnchor:
		return appleAnchored();
	case opAnchorHash:
		{
			SecCertificateRef cert = mContext->cert(get<int32_t>());
			return verifyAnchor(cert, getSHA1());
		}
	case opInfoKeyValue:	// [legacy; use opInfoKeyField]
		{
			string key = getString();
			return infoKeyValue(key, Match(CFTempString(getString()), matchEqual));
		}
	case opAnd:
		return eval(depth) & eval(depth);
	case opOr:
		return eval(depth) | eval(depth);
	case opCDHash:
		if (mContext->directory) {
			CFRef<CFDataRef> cdhash = mContext->directory->cdhash();
			CFRef<CFDataRef> required = getHash();
			return CFEqual(cdhash, required);
		} else
			return false;
	case opNot:
		return !eval(depth);
	case opInfoKeyField:
		{
			string key = getString();
			Match match(*this);
			return infoKeyValue(key, match);
		}
	case opEntitlementField:
		{
			string key = getString();
			Match match(*this);
			return entitlementValue(key, match);
		}
	case opCertField:
		{
			SecCertificateRef cert = mContext->cert(get<int32_t>());
			string key = getString();
			Match match(*this);
			return certFieldValue(key, match, cert);
		}
#if TARGET_OS_OSX
	case opCertGeneric:
		{
			SecCertificateRef cert = mContext->cert(get<int32_t>());
			string key = getString();
			Match match(*this);
			return certFieldGeneric(key, match, cert);
		}
	case opCertFieldDate:
		{
			SecCertificateRef cert = mContext->cert(get<int32_t>());
			string key = getString();
			Match match(*this);
			return certFieldDate(key, match, cert);
		}
	case opCertPolicy:
		{
			SecCertificateRef cert = mContext->cert(get<int32_t>());
			string key = getString();
			Match match(*this);
			return certFieldPolicy(key, match, cert);
		}
#endif
	case opTrustedCert:
		return trustedCert(get<int32_t>());
	case opTrustedCerts:
		return trustedCerts();
	case opNamedAnchor:
		return fragments().namedAnchor(getString(), *mContext);
	case opNamedCode:
		return fragments().named(getString(), *mContext);
	case opPlatform:
		{
			int32_t targetPlatform = get<int32_t>();
			return mContext->directory && mContext->directory->platform == targetPlatform;
		}
	case opNotarized:
		{
			return isNotarized(mContext);
		}
	default:
		// opcode not recognized - handle generically if possible, fail otherwise
		if (op & (opGenericFalse | opGenericSkip)) {
			// unknown opcode, but it has a size field and can be safely bypassed
			skip(get<uint32_t>());
			if (op & opGenericFalse) {
				CODESIGN_EVAL_REQINT_UNKNOWN_FALSE(op);
				return false;
			} else {
				CODESIGN_EVAL_REQINT_UNKNOWN_SKIPPED(op);
				return eval(depth);
			}
		}
		// unrecognized opcode and no way to interpret it
		secinfo("csinterp", "opcode 0x%x cannot be handled; aborting", op);
		MacOSError::throwMe(errSecCSUnimplemented);
	}
}


//
// Evaluate an Info.plist key condition
//
bool Requirement::Interpreter::infoKeyValue(const string &key, const Match &match)
{
	if (mContext->info)		// we have an Info.plist
		if (CFTypeRef value = CFDictionaryGetValue(mContext->info, CFTempString(key)))
			return match(value);
	return match(kCFNull);
}


//
// Evaluate an entitlement condition
//
bool Requirement::Interpreter::entitlementValue(const string &key, const Match &match)
{
	if (mContext->entitlements)		// we have an Info.plist
		if (CFTypeRef value = CFDictionaryGetValue(mContext->entitlements, CFTempString(key)))
			return match(value);
	return match(kCFNull);
}


bool Requirement::Interpreter::certFieldValue(const string &key, const Match &match, SecCertificateRef cert)
{
// XXX: Not supported on embedded yet due to lack of supporting API
#if TARGET_OS_OSX
	// no cert, no chance
	if (cert == NULL)
		return false;

	// a table of recognized keys for the "certificate[foo]" syntax
	static const struct CertField {
		const char *name;
		const CSSM_OID *oid;
	} certFields[] = {
		{ "subject.C", &CSSMOID_CountryName },
		{ "subject.CN", &CSSMOID_CommonName },
		{ "subject.D", &CSSMOID_Description },
		{ "subject.L", &CSSMOID_LocalityName },
//		{ "subject.C-L", &CSSMOID_CollectiveLocalityName },	// missing from Security.framework headers
		{ "subject.O", &CSSMOID_OrganizationName },
		{ "subject.C-O", &CSSMOID_CollectiveOrganizationName },
		{ "subject.OU", &CSSMOID_OrganizationalUnitName },
		{ "subject.C-OU", &CSSMOID_CollectiveOrganizationalUnitName },
		{ "subject.ST", &CSSMOID_StateProvinceName },
		{ "subject.C-ST", &CSSMOID_CollectiveStateProvinceName },
		{ "subject.STREET", &CSSMOID_StreetAddress },
		{ "subject.C-STREET", &CSSMOID_CollectiveStreetAddress },
		{ "subject.UID", &CSSMOID_UserID },
		{ NULL, NULL }
	};

	// DN-component single-value match
	for (const CertField *cf = certFields; cf->name; cf++)
		if (cf->name == key) {
			CFRef<CFStringRef> value;
            OSStatus rc = SecCertificateCopySubjectComponent(cert, cf->oid, &value.aref());
			if (rc) {
				secinfo("csinterp", "cert %p lookup for DN.%s failed rc=%d", cert, key.c_str(), (int)rc);
				return false;
			}
			return match(value);
		}

	// email multi-valued match (any of...)
	if (key == "email") {
		CFRef<CFArrayRef> value;
        OSStatus rc = SecCertificateCopyEmailAddresses(cert, &value.aref());
		if (rc) {
			secinfo("csinterp", "cert %p lookup for email failed rc=%d", cert, (int)rc);
			return false;
		}
		return match(value);
	}

	// unrecognized key. Fail but do not abort to promote backward compatibility down the road
	secinfo("csinterp", "cert field notation \"%s\" not understood", key.c_str());
#endif
	return false;
}

#if TARGET_OS_OSX
bool Requirement::Interpreter::certFieldGeneric(const string &key, const Match &match, SecCertificateRef cert)
{
	// the key is actually a (binary) OID value
	CssmOid oid((char *)key.data(), key.length());
	return certFieldGeneric(oid, match, cert);
}

bool Requirement::Interpreter::certFieldGeneric(const CssmOid &oid, const Match &match, SecCertificateRef cert)
{
	return cert && match(certificateHasField(cert, oid) ? (CFTypeRef)kCFBooleanTrue : (CFTypeRef)kCFNull);
}

bool Requirement::Interpreter::certFieldDate(const string &key, const Match &match, SecCertificateRef cert)
{
	// the key is actually a (binary) OID value
	CssmOid oid((char *)key.data(), key.length());
	return certFieldDate(oid, match, cert);
}
	
bool Requirement::Interpreter::certFieldDate(const CssmOid &oid, const Match &match, SecCertificateRef cert)
{
	CFTypeRef value = cert != NULL ? certificateCopyFieldDate(cert, oid) : NULL;
	bool matching = match(value != NULL ? value : kCFNull);
	
	if (value) {
		CFRelease(value);
	}
	
	return matching;
}

bool Requirement::Interpreter::certFieldPolicy(const string &key, const Match &match, SecCertificateRef cert)
{
	// the key is actually a (binary) OID value
	CssmOid oid((char *)key.data(), key.length());
	return certFieldPolicy(oid, match, cert);
}

bool Requirement::Interpreter::certFieldPolicy(const CssmOid &oid, const Match &match, SecCertificateRef cert)
{
	return cert && match(certificateHasPolicy(cert, oid) ? (CFTypeRef)kCFBooleanTrue : (CFTypeRef)kCFNull);
}
#endif

//
// Check the Apple-signed condition
//
bool Requirement::Interpreter::appleAnchored()
{
	if (SecCertificateRef cert = mContext->cert(anchorCert))
		if (isAppleCA(cert))
		return true;
	return false;
}

static CFStringRef kAMFINVRAMTrustedKeys = CFSTR("AMFITrustedKeys");

CFArrayRef Requirement::Interpreter::getAdditionalTrustedAnchors()
{
    __block CFRef<CFMutableArrayRef> keys = makeCFMutableArray(0);

    try {
        io_registry_entry_t entry = IORegistryEntryFromPath(kIOMasterPortDefault, "IODeviceTree:/options");
        if (entry == IO_OBJECT_NULL)
            return NULL;

        CFRef<CFDataRef> configData = (CFDataRef)IORegistryEntryCreateCFProperty(entry, kAMFINVRAMTrustedKeys, kCFAllocatorDefault, 0);
        IOObjectRelease(entry);
        if (!configData)
            return NULL;

        CFRef<CFDictionaryRef> configDict = CFDictionaryRef(IOCFUnserializeWithSize((const char *)CFDataGetBytePtr(configData),
                                                                                    (size_t)CFDataGetLength(configData),
                                                                                    kCFAllocatorDefault, 0, NULL));
        if (!configDict)
            return NULL;

        CFArrayRef trustedKeys = CFArrayRef(CFDictionaryGetValue(configDict, CFSTR("trustedKeys")));
        if (!trustedKeys && CFGetTypeID(trustedKeys) != CFArrayGetTypeID())
            return NULL;

        cfArrayApplyBlock(trustedKeys, ^(const void *value) {
            CFDictionaryRef key = CFDictionaryRef(value);
            if (!key && CFGetTypeID(key) != CFDictionaryGetTypeID())
                return;

            CFDataRef hash = CFDataRef(CFDictionaryGetValue(key, CFSTR("certDigest")));
            if (!hash && CFGetTypeID(hash) != CFDataGetTypeID())
                return;
            CFArrayAppendValue(keys, hash);
        });

    } catch (...) {
    }

    if (CFArrayGetCount(keys) == 0)
        return NULL;

    return keys.yield();
}

bool Requirement::Interpreter::appleLocalAnchored()
{
    static CFArrayRef additionalTrustedCertificates = NULL;

    if (csr_check(CSR_ALLOW_APPLE_INTERNAL))
        return false;

	if (mContext->forcePlatform) {
		return true;
	}

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        additionalTrustedCertificates = getAdditionalTrustedAnchors();
    });

    if (additionalTrustedCertificates == NULL)
        return false;

    CFRef<CFDataRef> hash = SecCertificateCopySHA256Digest(mContext->cert(leafCert));
    if (!hash)
        return false;

    if (CFArrayContainsValue(additionalTrustedCertificates, CFRangeMake(0, CFArrayGetCount(additionalTrustedCertificates)), hash))
        return true;

    return false;
}

bool Requirement::Interpreter::appleSigned()
{
    if (appleAnchored()) {
		if (SecCertificateRef intermed = mContext->cert(-2))	// first intermediate
			// first intermediate common name match (exact)
			if (certFieldValue("subject.CN", Match(appleIntermediateCN, matchEqual), intermed)
					&& certFieldValue("subject.O", Match(appleIntermediateO, matchEqual), intermed))
				return true;
    } else if (appleLocalAnchored()) {
        return true;
	}
	return false;
}


//
// Verify an anchor requirement against the context
//
bool Requirement::Interpreter::verifyAnchor(SecCertificateRef cert, const unsigned char *digest)
{
	// get certificate bytes
	if (cert) {
        SHA1 hasher;
#if TARGET_OS_OSX
		CSSM_DATA certData;
		MacOSError::check(SecCertificateGetData(cert, &certData));
		
		// verify hash
		hasher(certData.Data, certData.Length);
#else
        hasher(SecCertificateGetBytePtr(cert), SecCertificateGetLength(cert));
#endif
		return hasher.verify(digest);
	}
	return false;
}


//
// Check one or all certificate(s) in the cert chain against the Trust Settings database.
//
bool Requirement::Interpreter::trustedCerts()
{
	int anchor = mContext->certCount() - 1;
	for (int slot = 0; slot <= anchor; slot++)
		if (SecCertificateRef cert = mContext->cert(slot))
			switch (trustSetting(cert, slot == anchor)) {
			case kSecTrustSettingsResultTrustRoot:
			case kSecTrustSettingsResultTrustAsRoot:
				return true;
			case kSecTrustSettingsResultDeny:
				return false;
			case kSecTrustSettingsResultUnspecified:
				break;
			default:
				assert(false);
				return false;
			}
		else
			return false;
	return false;
}

bool Requirement::Interpreter::trustedCert(int slot)
{
	if (SecCertificateRef cert = mContext->cert(slot)) {
		int anchorSlot = mContext->certCount() - 1;
		switch (trustSetting(cert, slot == anchorCert || slot == anchorSlot)) {
		case kSecTrustSettingsResultTrustRoot:
		case kSecTrustSettingsResultTrustAsRoot:
			return true;
		case kSecTrustSettingsResultDeny:
		case kSecTrustSettingsResultUnspecified:
			return false;
		default:
			assert(false);
			return false;
		}
	} else
		return false;
}


//
// Explicitly check one certificate against the Trust Settings database and report
// the findings. This is a helper for the various Trust Settings evaluators.
//
SecTrustSettingsResult Requirement::Interpreter::trustSetting(SecCertificateRef cert, bool isAnchor)
{
    // XXX: Not supported on embedded yet due to lack of supporting API
#if TARGET_OS_OSX
	// the SPI input is the uppercase hex form of the SHA-1 of the certificate...
	assert(cert);
	SHA1::Digest digest;
	hashOfCertificate(cert, digest);
	string Certhex = CssmData(digest, sizeof(digest)).toHex();
	for (string::iterator it = Certhex.begin(); it != Certhex.end(); ++it)
		if (islower(*it))
			*it = toupper(*it);
	
	// call Trust Settings and see what it finds
	SecTrustSettingsDomain domain;
	SecTrustSettingsResult result;
	CSSM_RETURN *errors = NULL;
	uint32 errorCount = 0;
	bool foundMatch, foundAny;
	switch (OSStatus rc = SecTrustSettingsEvaluateCert(
		CFTempString(Certhex),					// settings index
		&CSSMOID_APPLE_TP_CODE_SIGNING,			// standard code signing policy
		NULL, 0,								// policy string (unused)
		kSecTrustSettingsKeyUseAny,				// no restriction on key usage @@@
		isAnchor,								// consult system default anchor set

		&domain,								// domain of found setting
		&errors, &errorCount,					// error set and maximum count
		&result,								// the actual setting
		&foundMatch, &foundAny					// optimization hints (not used)
		)) {
	case errSecSuccess:
		::free(errors);
		if (foundMatch)
			return result;
		else
			return kSecTrustSettingsResultUnspecified;
	default:
		::free(errors);
		MacOSError::throwMe(rc);
	}
#else
    return kSecTrustSettingsResultUnspecified;
#endif
}


//
// Create a Match object from the interpreter stream
//
Requirement::Interpreter::Match::Match(Interpreter &interp)
{
	switch (mOp = interp.get<MatchOperation>()) {
	case matchAbsent:
	case matchExists:
		break;
	case matchEqual:
	case matchContains:
	case matchBeginsWith:
	case matchEndsWith:
	case matchLessThan:
	case matchGreaterThan:
	case matchLessEqual:
	case matchGreaterEqual:
		mValue.take(makeCFString(interp.getString()));
		break;
	case matchOn:
	case matchBefore:
	case matchAfter:
	case matchOnOrBefore:
	case matchOnOrAfter: {
		mValue.take(CFDateCreate(NULL, interp.getAbsoluteTime()));
		break;
	}
	default:
		// Assume this (unknown) match type has a single data argument.
		// This gives us a chance to keep the instruction stream aligned.
		interp.getString();			// discard
		break;
	}
}


//
// Execute a match against a candidate value
//
bool Requirement::Interpreter::Match::operator () (CFTypeRef candidate) const
{
	// null candidates always fail
	if (!candidate)
		return false;

	if (candidate == kCFNull) {
		return mOp == matchAbsent; // only 'absent' matches
	}
	
	// interpret an array as matching alternatives (any one succeeds)
	if (CFGetTypeID(candidate) == CFArrayGetTypeID()) {
		CFArrayRef array = CFArrayRef(candidate);
		CFIndex count = CFArrayGetCount(array);
		for (CFIndex n = 0; n < count; n++)
			if ((*this)(CFArrayGetValueAtIndex(array, n)))	// yes, it's recursive
				return true;
	}

	switch (mOp) {
	case matchAbsent:
		return false;		// it exists, so it cannot be absent
	case matchExists:		// anything but NULL and boolean false "exists"
		return !CFEqual(candidate, kCFBooleanFalse);
	case matchEqual:		// equality works for all CF types
		return CFEqual(candidate, mValue);
	case matchContains:
		if (isStringValue() && CFGetTypeID(candidate) == CFStringGetTypeID()) {
			CFStringRef value = CFStringRef(candidate);
			if (CFStringFindWithOptions(value, cfStringValue(), CFRangeMake(0, CFStringGetLength(value)), 0, NULL))
				return true;
		}
		return false;
	case matchBeginsWith:
		if (isStringValue() && CFGetTypeID(candidate) == CFStringGetTypeID()) {
			CFStringRef value = CFStringRef(candidate);
			if (CFStringFindWithOptions(value, cfStringValue(), CFRangeMake(0, CFStringGetLength(cfStringValue())), 0, NULL))
				return true;
		}
		return false;
	case matchEndsWith:
		if (isStringValue() && CFGetTypeID(candidate) == CFStringGetTypeID()) {
			CFStringRef value = CFStringRef(candidate);
			CFIndex matchLength = CFStringGetLength(cfStringValue());
			CFIndex start = CFStringGetLength(value) - matchLength;
			if (start >= 0)
				if (CFStringFindWithOptions(value, cfStringValue(), CFRangeMake(start, matchLength), 0, NULL))
					return true;
		}
		return false;
	case matchLessThan:
		return inequality(candidate, kCFCompareNumerically, kCFCompareLessThan, true);
	case matchGreaterThan:
		return inequality(candidate, kCFCompareNumerically, kCFCompareGreaterThan, true);
	case matchLessEqual:
		return inequality(candidate, kCFCompareNumerically, kCFCompareGreaterThan, false);
	case matchGreaterEqual:
		return inequality(candidate, kCFCompareNumerically, kCFCompareLessThan, false);
	case matchOn:
	case matchBefore:
	case matchAfter:
	case matchOnOrBefore:
	case matchOnOrAfter: {
		if (!isDateValue() || CFGetTypeID(candidate) != CFDateGetTypeID()) {
			return false;
		}
		
		CFComparisonResult res = CFDateCompare((CFDateRef)candidate, cfDateValue(), NULL);

		switch (mOp) {
			case matchOn: return res == 0;
			case matchBefore: return res < 0;
			case matchAfter: return res > 0;
			case matchOnOrBefore: return res <= 0;
			case matchOnOrAfter: return res >= 0;
			default: abort();
		}
	}
	default:
		// unrecognized match types can never match
		return false;
	}
}


bool Requirement::Interpreter::Match::inequality(CFTypeRef candidate, CFStringCompareFlags flags,
	CFComparisonResult outcome, bool negate) const
{
	if (isStringValue() && CFGetTypeID(candidate) == CFStringGetTypeID()) {
		CFStringRef value = CFStringRef(candidate);
		if ((CFStringCompare(value, cfStringValue(), flags) == outcome) == negate)
			return true;
	}
	return false;
}


//
// External fragments
//
Fragments::Fragments()
{
	mMyBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.security"));
}


bool Fragments::evalNamed(const char *type, const std::string &name, const Requirement::Context &ctx)
{
	if (CFDataRef fragData = fragment(type, name)) {
		const Requirement *req = (const Requirement *)CFDataGetBytePtr(fragData);	// was prevalidated as Requirement
		return req->validates(ctx);
	}
	return false;
}


CFDataRef Fragments::fragment(const char *type, const std::string &name)
{
	string key = name + "!!" + type;	// compound key
	StLock<Mutex> _(mLock);				// lock for cache access
	FragMap::const_iterator it = mFragments.find(key);
	if (it == mFragments.end()) {
		CFRef<CFDataRef> fragData;		// will always be set (NULL on any errors)
		if (CFRef<CFURLRef> fragURL = CFBundleCopyResourceURL(mMyBundle, CFTempString(name), CFSTR("csreq"), CFTempString(type)))
			if (CFRef<CFDataRef> data = cfLoadFile(fragURL)) {	// got data
				const Requirement *req = (const Requirement *)CFDataGetBytePtr(data);
				if (req->validateBlob(CFDataGetLength(data)))	// looks like a Requirement...
					fragData = data;			// ... so accept it
				else
					Syslog::warning("Invalid sub-requirement at %s", cfString(fragURL).c_str());
			}
		if (CODESIGN_EVAL_REQINT_FRAGMENT_LOAD_ENABLED())
			CODESIGN_EVAL_REQINT_FRAGMENT_LOAD(type, name.c_str(), fragData ? CFDataGetBytePtr(fragData) : NULL);
		mFragments[key] = fragData;		// cache it, success or failure
		return fragData;
	}
	CODESIGN_EVAL_REQINT_FRAGMENT_HIT(type, name.c_str());
	return it->second;
}


}	// CodeSigning
}	// Security
