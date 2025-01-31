/*
 * Copyright (c) 2007-2018 Apple Inc. All Rights Reserved.
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

/* 
 * SecTrustStore.c - CertificateSource API to a system root certificate store
 */
#include <Security/SecTrustStore.h>

#include <Security/SecCertificateInternal.h>
#include <Security/SecInternal.h>
#include <Security/SecuritydXPC.h>
#include <Security/SecPolicyPriv.h>
#include <Security/SecPolicyInternal.h>
#include <CoreFoundation/CFString.h>
#include <AssertMacros.h>
#include <ipc/securityd_client.h>
#include "SecFramework.h"
#include <sys/stat.h>
#include <stdio.h>
#include <os/activity.h>
#include <dirent.h>
#include "SecTrustPriv.h"
#include <Security/SecTrustSettingsPriv.h>
#include <utilities/SecCFError.h>
#include <utilities/SecCFWrappers.h>
#include "utilities/SecDb.h"
#include "SecTrustInternal.h"

static CFStringRef kSecTrustStoreUserName = CFSTR("user");

SecTrustStoreRef SecTrustStoreForDomain(SecTrustStoreDomain domain) {
    CFStringRef domainName;
    if (domain == kSecTrustStoreDomainUser) {
        domainName = kSecTrustStoreUserName;
    } else {
        return NULL;
    }

    if (gTrustd) {
        return gTrustd->sec_trust_store_for_domain(domainName, NULL);
    } else {
        return (SecTrustStoreRef)domainName;
    }
}

static bool string_data_to_bool_error(enum SecXPCOperation op, SecTrustStoreRef ts, CFDataRef digest, CFErrorRef *error)
{
    return securityd_send_sync_and_do(op, error, ^bool(xpc_object_t message, CFErrorRef *error) {
        return SecXPCDictionarySetString(message, kSecXPCKeyDomain, (CFStringRef)ts, error) &&
        SecXPCDictionarySetData(message, kSecXPCKeyDigest, digest, error);
    }, NULL);
}

static bool string_data_to_bool_bool_error(enum SecXPCOperation op, SecTrustStoreRef ts, CFDataRef digest, bool *result, CFErrorRef *error)
{
    os_activity_t activity = os_activity_create("SecTrustStoreContains", OS_ACTIVITY_CURRENT, OS_ACTIVITY_FLAG_DEFAULT);
    os_activity_scope(activity);
    bool status = securityd_send_sync_and_do(op, error, ^bool(xpc_object_t message, CFErrorRef *error) {
        return SecXPCDictionarySetString(message, kSecXPCKeyDomain, (CFStringRef)ts, error) &&
        SecXPCDictionarySetData(message, kSecXPCKeyDigest, digest, error);
    }, ^bool(xpc_object_t response, CFErrorRef *error) {
        if (result)
            *result = xpc_dictionary_get_bool(response, kSecXPCKeyResult);
        return true;
    });
    os_release(activity);
    return status;
}

Boolean SecTrustStoreContains(SecTrustStoreRef ts,
	SecCertificateRef certificate) {
    CFDataRef digest;
    bool ok = false;
	__block bool contains = false;

	require(ts, errOut);
	require(digest = SecCertificateGetSHA1Digest(certificate), errOut);
    

    ok = (SecOSStatusWith(^bool (CFErrorRef *error) {
        return TRUSTD_XPC(sec_trust_store_contains, string_data_to_bool_bool_error, ts, digest, &contains, error);
    }) == errSecSuccess);
    
errOut:
	return ok && contains;
}

static bool SecXPCDictionarySetCertificate(xpc_object_t message, const char *key, SecCertificateRef certificate, CFErrorRef *error) {
    if (certificate) {
        xpc_dictionary_set_data(message, key, SecCertificateGetBytePtr(certificate),
                                SecCertificateGetLength(certificate));
        return true;
    }
    return SecError(errSecParam, error, CFSTR("NULL certificate"));
}


static bool string_cert_cftype_to_error(enum SecXPCOperation op, SecTrustStoreRef ts, SecCertificateRef certificate, CFTypeRef trustSettingsDictOrArray, CFErrorRef *error)
{
    return securityd_send_sync_and_do(op, error, ^bool(xpc_object_t message, CFErrorRef *error) {
        bool ok = false;
        ok = SecXPCDictionarySetString(message, kSecXPCKeyDomain, (CFStringRef)ts, error) &&
        SecXPCDictionarySetCertificate(message, kSecXPCKeyCertificate, certificate, error) &&
        (!trustSettingsDictOrArray || SecXPCDictionarySetPList(message, kSecXPCKeySettings, trustSettingsDictOrArray, error));
        return ok;
    }, NULL);
}

static OSStatus validateConstraint(Boolean isSelfSigned, CFMutableDictionaryRef trustSettingsDict) {
    OSStatus result = errSecSuccess;

    /* Check "TrustRoot"/"TrustAsRoot" */
    CFNumberRef resultNumber = NULL;
    resultNumber = (CFNumberRef)CFDictionaryGetValue(trustSettingsDict, kSecTrustSettingsResult);
    uint32_t resultValue = kSecTrustSettingsResultInvalid;
    if (!isNumber(resultNumber) && !isSelfSigned) {
        /* only self-signed certs get default of TrustAsRoot */
        return errSecParam;
    }
    if (isNumber(resultNumber) && CFNumberGetValue(resultNumber, kCFNumberSInt32Type, &resultValue)) {
        if (isSelfSigned && resultValue == kSecTrustSettingsResultTrustAsRoot) {
            return errSecParam;
        }
        if (!isSelfSigned && resultValue == kSecTrustSettingsResultTrustRoot) {
            return errSecParam;
        }
    }

    /* If there's a policy specified, change the contents */
    SecPolicyRef policy = NULL;
    policy = (SecPolicyRef)CFDictionaryGetValue(trustSettingsDict, kSecTrustSettingsPolicy);
    if (policy) {
        CFStringRef policyOid = NULL, policyName = NULL;
        policyOid = SecPolicyGetOidString(policy);
        policyName = SecPolicyGetName(policy);
        CFDictionarySetValue(trustSettingsDict, kSecTrustSettingsPolicy, policyOid);
        if (policyName) { CFDictionaryAddValue(trustSettingsDict, kSecTrustSettingsPolicyName, policyName); }
    }

    return result;
}

static OSStatus validateTrustSettings(Boolean isSelfSigned,
            CFTypeRef trustSettingsDictOrArray,
            CFTypeRef * CF_RETURNS_RETAINED modifiedTrustSettings) {
    OSStatus status = errSecParam;
    CFTypeRef result = NULL;

    /* NULL is a valid input */
    if (!trustSettingsDictOrArray && isSelfSigned) {
        return errSecSuccess;
    } else if (!trustSettingsDictOrArray && !isSelfSigned) {
        return errSecParam;
    }

    if (CFDictionaryGetTypeID() == CFGetTypeID(trustSettingsDictOrArray)) {
        result = CFDictionaryCreateMutableCopy(NULL, 0, trustSettingsDictOrArray);
        status = validateConstraint(isSelfSigned, (CFMutableDictionaryRef)result);
    } else if (CFArrayGetTypeID() == CFGetTypeID(trustSettingsDictOrArray)) {
        require_action_quiet(result = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks),
                             out, status = errSecAllocate);
        CFIndex ix, count = CFArrayGetCount(trustSettingsDictOrArray);
        for (ix = 0; ix < count; ix++) {
            CFDictionaryRef constraint = CFArrayGetValueAtIndex(trustSettingsDictOrArray, ix);
            CFDictionaryRef modifiedConstraint = NULL;
            require_noerr_quiet(status = validateTrustSettings(isSelfSigned, constraint, (CFTypeRef *)&modifiedConstraint), out);
            CFArrayAppendValue((CFMutableArrayRef)result, modifiedConstraint);
            CFReleaseNull(modifiedConstraint); /* constraint now owned by array */
        }
    }

out:
    if (errSecSuccess == status && modifiedTrustSettings) {
        *modifiedTrustSettings = CFRetainSafe(result);
    }
    CFReleaseNull(result);
    return status;
}

OSStatus SecTrustStoreSetTrustSettings(SecTrustStoreRef ts,
	SecCertificateRef certificate,
    CFTypeRef trustSettingsDictOrArray) {
    __block OSStatus result;
    __block CFTypeRef validatedTrustSettings = NULL;

    Boolean isSelfSigned = false;
    require_noerr_quiet(result = SecCertificateIsSelfSigned(certificate, &isSelfSigned), out);
    require_noerr_quiet(result = validateTrustSettings(isSelfSigned, trustSettingsDictOrArray, &validatedTrustSettings), out);
    
    os_activity_initiate("SecTrustStoreSetTrustSettings", OS_ACTIVITY_FLAG_DEFAULT, ^{
        result = SecOSStatusWith(^bool (CFErrorRef *error) {
            return TRUSTD_XPC(sec_trust_store_set_trust_settings, string_cert_cftype_to_error, ts, certificate, validatedTrustSettings, error);
        });
    });

out:
    CFReleaseNull(validatedTrustSettings);
    return result;
}

OSStatus SecTrustStoreRemoveCertificate(SecTrustStoreRef ts,
    SecCertificateRef certificate)
{
    CFDataRef digest;
    __block OSStatus status = errSecParam;

    os_activity_t activity = os_activity_create("SecTrustStoreRemoveCertificate", OS_ACTIVITY_CURRENT, OS_ACTIVITY_FLAG_DEFAULT);
    os_activity_scope(activity);
    require(ts, errOut);
    require(digest = SecCertificateGetSHA1Digest(certificate), errOut);
    require(gTrustd || ts == (SecTrustStoreRef)kSecTrustStoreUserName, errOut);
    
    status = SecOSStatusWith(^bool (CFErrorRef *error) {
        return TRUSTD_XPC(sec_trust_store_remove_certificate, string_data_to_bool_error, ts, digest, error);
    });

errOut:
    os_release(activity);
	return status;
}

OSStatus SecTrustStoreGetSettingsVersionNumber(SecTrustSettingsVersionNumber* p_settings_version_number)
{
    if (NULL == p_settings_version_number) {
        return errSecParam;
    }

    OSStatus status = errSecSuccess;
    CFErrorRef error = nil;
    uint64_t versionNumber = SecTrustGetTrustStoreVersionNumber(&error);
    *p_settings_version_number = (SecTrustSettingsVersionNumber)versionNumber;

    if (error) {
        status = (OSStatus)CFErrorGetCode(error);
    }
    CFReleaseSafe(error);
    return status;
}

OSStatus SecTrustStoreGetSettingsAssetVersionNumber(SecTrustSettingsAssetVersionNumber* p_settings_asset_version_number)
{
    if (NULL == p_settings_asset_version_number) {
        return errSecParam;
    }

    OSStatus status = errSecSuccess;
    CFErrorRef error = nil;
    uint64_t versionNumber = SecTrustGetAssetVersionNumber(&error);
    *p_settings_asset_version_number = (SecTrustSettingsAssetVersionNumber)versionNumber;

    if (error) {
        status = (OSStatus)CFErrorGetCode(error);
    }
    CFReleaseSafe(error);
    return status;
}

static bool string_to_array_error(enum SecXPCOperation op, SecTrustStoreRef ts, CFArrayRef *trustStoreContents, CFErrorRef *error)
{
    return securityd_send_sync_and_do(op, error, ^bool(xpc_object_t message, CFErrorRef *error) {
        return SecXPCDictionarySetString(message, kSecXPCKeyDomain, (CFStringRef)ts, error);
    }, ^bool(xpc_object_t response, CFErrorRef *error) {
        if (trustStoreContents) {
            *trustStoreContents = SecXPCDictionaryCopyArray(response, kSecXPCKeyResult, error);
            if (!*trustStoreContents) return false;
        }
        return true;
    });
}

OSStatus SecTrustStoreCopyAll(SecTrustStoreRef ts, CFArrayRef *trustStoreContents)
{
    __block CFArrayRef results = NULL;
    OSStatus status = errSecParam;

    os_activity_t activity = os_activity_create("SecTrustStoreCopyAll", OS_ACTIVITY_CURRENT, OS_ACTIVITY_FLAG_DEFAULT);
    os_activity_scope(activity);
    require(ts, errOut);

    status = SecOSStatusWith(^bool (CFErrorRef *error) {
        return TRUSTD_XPC(sec_trust_store_copy_all, string_to_array_error, ts, &results, error);
    });

    *trustStoreContents = results;

errOut:
    os_release(activity);
    return status;
}

static bool string_data_to_array_error(enum SecXPCOperation op, SecTrustStoreRef ts, CFDataRef digest, CFArrayRef *usageConstraints, CFErrorRef *error)
{
    return securityd_send_sync_and_do(op, error, ^bool(xpc_object_t message, CFErrorRef *error) {
        return SecXPCDictionarySetString(message, kSecXPCKeyDomain, (CFStringRef)ts, error) &&
        SecXPCDictionarySetData(message, kSecXPCKeyDigest, digest, error);
    }, ^bool(xpc_object_t response, CFErrorRef *error) {
        return SecXPCDictionaryCopyArrayOptional(response, kSecXPCKeyResult, usageConstraints, error);
    });
}

OSStatus SecTrustStoreCopyUsageConstraints(SecTrustStoreRef ts, SecCertificateRef certificate, CFArrayRef *usageConstraints)
{
    CFDataRef digest;
    __block CFArrayRef results = NULL;
    OSStatus status = errSecParam;

    os_activity_t activity = os_activity_create("SecTrustStoreCopyUsageConstraints", OS_ACTIVITY_CURRENT, OS_ACTIVITY_FLAG_DEFAULT);
    os_activity_scope(activity);
    require(ts, errOut);
    require(certificate, errOut);
    require(digest = SecCertificateGetSHA1Digest(certificate), errOut);
    require(usageConstraints, errOut);

    status = SecOSStatusWith(^bool (CFErrorRef *error) {
        return TRUSTD_XPC(sec_trust_store_copy_usage_constraints, string_data_to_array_error, ts, digest, &results, error);
    });

    *usageConstraints = results;

errOut:
    os_release(activity);
    return status;
}

#define do_if_registered(sdp, ...) if (gTrustd && gTrustd->sdp) { return gTrustd->sdp(__VA_ARGS__); }

/* MARK: CT Enforcement Exceptions */

const CFStringRef kSecCTExceptionsCAsKey = CFSTR("DisabledForCAs");
const CFStringRef kSecCTExceptionsDomainsKey = CFSTR("DisabledForDomains");
const CFStringRef kSecCTExceptionsHashAlgorithmKey = CFSTR("HashAlgorithm");
const CFStringRef kSecCTExceptionsSPKIHashKey = CFSTR("SubjectPublicKeyInfoHash");

bool SecTrustStoreSetCTExceptions(CFStringRef applicationIdentifier, CFDictionaryRef exceptions, CFErrorRef *error) {
    do_if_registered(sec_trust_store_set_ct_exceptions, applicationIdentifier, exceptions, error);

    os_activity_t activity = os_activity_create("SecTrustStoreSetCTExceptions", OS_ACTIVITY_CURRENT, OS_ACTIVITY_FLAG_DEFAULT);
    os_activity_scope(activity);

    __block bool result = false;
    securityd_send_sync_and_do(kSecXPCOpSetCTExceptions, error, ^bool(xpc_object_t message, CFErrorRef *block_error) {
        SecXPCDictionarySetPListOptional(message, kSecTrustExceptionsKey, exceptions, block_error);
        SecXPCDictionarySetStringOptional(message, kSecTrustEventApplicationID, applicationIdentifier, block_error);
        return true;
    }, ^bool(xpc_object_t response, CFErrorRef *block_error) {
        result = SecXPCDictionaryGetBool(response, kSecXPCKeyResult, block_error);
        return true;
    });

    os_release(activity);
    return result;
}

CFDictionaryRef SecTrustStoreCopyCTExceptions(CFStringRef applicationIdentifier, CFErrorRef *error) {
    do_if_registered(sec_trust_store_copy_ct_exceptions, applicationIdentifier, error);

    os_activity_t activity = os_activity_create("SecTrustStoreCopyCTExceptions", OS_ACTIVITY_CURRENT, OS_ACTIVITY_FLAG_DEFAULT);
    os_activity_scope(activity);

    __block CFDictionaryRef result = NULL;
    securityd_send_sync_and_do(kSecXPCOpCopyCTExceptions, error, ^bool(xpc_object_t message, CFErrorRef *block_error) {
        SecXPCDictionarySetStringOptional(message, kSecTrustEventApplicationID, applicationIdentifier, block_error);
        return true;
    }, ^bool(xpc_object_t response, CFErrorRef *block_error) {
        (void)SecXPCDictionaryCopyDictionaryOptional(response, kSecTrustExceptionsKey, &result, block_error);
        return true;
    });

    os_release(activity);
    return result;
}
