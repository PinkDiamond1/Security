/*
 * Copyright (c) 2005-2009,2011-2016 Apple Inc. All Rights Reserved.
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
 * oids.c - OID consts
 *
 */

#include <libDER/libDER.h>

#define OID_ISO_CCITT_DIR_SERVICE 			85
#define OID_DS              				OID_ISO_CCITT_DIR_SERVICE
#define OID_ATTR_TYPE        				OID_DS, 4
#define OID_EXTENSION        				OID_DS, 29
#define OID_ISO_STANDARD      	 			40
#define OID_ISO_MEMBER         				42
#define OID_US                 				OID_ISO_MEMBER, 134, 72

#define OID_ISO_IDENTIFIED_ORG 				43
#define OID_OSINET             				OID_ISO_IDENTIFIED_ORG, 4
#define OID_GOSIP              				OID_ISO_IDENTIFIED_ORG, 5
#define OID_DOD                				OID_ISO_IDENTIFIED_ORG, 6
#define OID_OIW                				OID_ISO_IDENTIFIED_ORG, 14

/* From the PKCS Standards */
#define OID_RSA               				OID_US, 134, 247, 13
#define OID_RSA_HASH          				OID_RSA, 2
#define OID_RSA_ENCRYPT       				OID_RSA, 3
#define OID_PKCS             				OID_RSA, 1
#define OID_PKCS_1          				OID_PKCS, 1
#define OID_PKCS_2          				OID_PKCS, 2
#define OID_PKCS_3          				OID_PKCS, 3
#define OID_PKCS_4          				OID_PKCS, 4
#define OID_PKCS_5          				OID_PKCS, 5
#define OID_PKCS_6          				OID_PKCS, 6
#define OID_PKCS_7          				OID_PKCS, 7
#define OID_PKCS_8          				OID_PKCS, 8
#define OID_PKCS_9          				OID_PKCS, 9
#define OID_PKCS_10         				OID_PKCS, 10
#define OID_PKCS_11          				OID_PKCS, 11
#define OID_PKCS_12          				OID_PKCS, 12

/* ANSI X9.62 */
#define OID_ANSI_X9_62						OID_US, 206, 61
#define OID_PUBLIC_KEY_TYPE					OID_ANSI_X9_62, 2
#define OID_EC_CURVE                        OID_ANSI_X9_62, 3, 1
#define OID_EC_SIG_TYPE                     OID_ANSI_X9_62, 4
#define OID_ECDSA_WITH_SHA2                 OID_EC_SIG_TYPE, 3

/* Certicom */
#define OID_CERTICOM						OID_ISO_IDENTIFIED_ORG, 132
#define OID_CERTICOM_EC_CURVE				OID_CERTICOM, 0

/* ANSI X9.42 */
#define OID_ANSI_X9_42						OID_US, 206, 62, 2
#define OID_ANSI_X9_42_SCHEME				OID_ANSI_X9_42, 3
#define OID_ANSI_X9_42_NAMED_SCHEME			OID_ANSI_X9_42, 4

/* ANSI X9.57 */
#define OID_ANSI_X9_57                      OID_US, 206, 56
#define OID_ANSI_X9_57_ALGORITHM            OID_ANSI_X9_57, 4

/* DOD IANA Security related objects. */
#define OID_IANA                            OID_DOD, 1, 5

/* Kerberos PKINIT */
#define OID_KERBv5							OID_IANA, 2
#define OID_KERBv5_PKINIT					OID_KERBv5, 3

/* DOD IANA Mechanisms. */
#define OID_MECHANISMS						OID_IANA, 5

/* PKIX */
#define OID_PKIX							OID_MECHANISMS, 7
#define OID_PE								OID_PKIX, 1
#define OID_QT								OID_PKIX, 2
#define OID_KP								OID_PKIX, 3
#define OID_OTHER_NAME						OID_PKIX, 8
#define OID_PDA								OID_PKIX, 9
#define OID_QCS								OID_PKIX, 11
#define OID_AD								OID_PKIX, 48
#define OID_AD_OCSP							OID_AD, 1
#define OID_AD_CAISSUERS                    OID_AD, 2

/* ISAKMP */
#define OID_ISAKMP							OID_MECHANISMS, 8

/* ETSI */
#define OID_ETSI							0x04, 0x00
#define OID_ETSI_QCS						0x04, 0x00, 0x8E, 0x46, 0x01

#define OID_OIW_SECSIG        				OID_OIW, 3

#define OID_OIW_ALGORITHM    				OID_OIW_SECSIG, 2

/* NIST defined digest algorithm arc (2, 16, 840, 1, 101, 3, 4, 2) */
#define OID_NIST_HASHALG					0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02

/*
 * Netscape OIDs.
 */
#define NETSCAPE_BASE_OID           0x60, 0x86, 0x48, 0x01, 0x86, 0xf8, 0x42

/*
 * Netscape cert extension.
 *
 *  netscape-cert-extension OBJECT IDENTIFIER ::=
 * 		{ 2 16 840 1 113730 1 }
 *
 *	BER = 06 08 60 86 48 01 86 F8 42 01
 */
#define NETSCAPE_CERT_EXTEN			NETSCAPE_BASE_OID, 0x01

#define NETSCAPE_CERT_POLICY		NETSCAPE_BASE_OID, 0x04

/*
 * Apple-specific OID bases
 */

/*
 * apple OBJECT IDENTIFIER ::=
 *     { iso(1) member-body(2) US(840) 113635 }
 *
 * BER = 06 06 2A 86 48 86 F7 63
 */
#define APPLE_OID                OID_US, 0x86, 0xf7, 0x63

/* appleDataSecurity OBJECT IDENTIFIER ::=
 *        { apple 100 }
 *      { 1 2 840 113635 100 }
 *
 * BER = 06 07 2A 86 48 86 F7 63 64
 */
#define APPLE_ADS_OID            APPLE_OID, 0x64

/*
 *    appleSecurityAlgorithm OBJECT IDENTIFIER ::=
 *        { appleDataSecurity 2 }
 *      { 1 2 840 113635 100 2 }
 *
 * BER = 06 08 2A 86 48 86 F7 63 64 02
 */
#define APPLE_ALG_OID            APPLE_ADS_OID, 2

/* Entrust OIDs. */
#define ENTRUST_BASE_OID            OID_US, 0x86, 0xf6, 0x7d

/*
 * Entrust cert extension.
 *
 *  entrust-cert-extension OBJECT IDENTIFIER ::=
 * 		{  1 2 840 113533 7 65 }
 *
 *	BER = 06 08 2A 86 48 86 F6 7D 07 41
 */
#define ENTRUST_CERT_EXTEN			ENTRUST_BASE_OID, 0x07, 0x41

/* Microsoft OIDs. */
#define MICROSOFT_BASE_OID          OID_DOD, 0x01, 0x04, 0x01, 0x82, 0x37
#define MICROSOFT_ENROLLMENT_OID    MICROSOFT_BASE_OID, 0x14

/* Google OIDs: 1.3.6.1.4.1.11129.
 */
#define GOOGLE_BASE_OID             OID_DOD, 0x01, 0x04, 0x01, 0xD6, 0x79
#define GOOGLE_EMBEDDED_SCT_OID     GOOGLE_BASE_OID, 0x02, 0x04, 0x02
#define GOOGLE_OCSP_SCT_OID         GOOGLE_BASE_OID, 0x02, 0x04, 0x05


/* Algorithm OIDs. */
static const DERByte
    _oidRsa[]                       = { OID_PKCS_1, 1 },
    _oidMd2Rsa[]                    = { OID_PKCS_1, 2 },
    _oidMd4Rsa[]                    = { OID_PKCS_1, 3 },
    _oidMd5Rsa[]                    = { OID_PKCS_1, 4 },
    _oidSha1Rsa[]                   = { OID_PKCS_1, 5 },
    _oidSha256Rsa[]                 = { OID_PKCS_1, 11 },   /* rfc5754 */
    _oidSha384Rsa[]                 = { OID_PKCS_1, 12 },   /* rfc5754 */
    _oidSha512Rsa[]                 = { OID_PKCS_1, 13 },   /* rfc5754 */
    _oidSha224Rsa[]                 = { OID_PKCS_1, 14 },   /* rfc5754 */
    _oidEcPubKey[]                  = { OID_PUBLIC_KEY_TYPE, 1 },
    _oidSha1Ecdsa[]                 = { OID_EC_SIG_TYPE, 1 },     /* rfc3279 */
    _oidSha224Ecdsa[]               = { OID_ECDSA_WITH_SHA2, 1 }, /* rfc5758 */
    _oidSha256Ecdsa[]               = { OID_ECDSA_WITH_SHA2, 2 }, /* rfc5758 */
    _oidSha384Ecdsa[]               = { OID_ECDSA_WITH_SHA2, 3 }, /* rfc5758 */
    _oidSha512Ecdsa[]               = { OID_ECDSA_WITH_SHA2, 4 }, /* rfc5758 */
    _oidSha1Dsa[]                   = { OID_ANSI_X9_57_ALGORITHM, 3 },
    _oidMd2[]                       = { OID_RSA_HASH, 2 },
    _oidMd4[]                       = { OID_RSA_HASH, 4 },
    _oidMd5[]                       = { OID_RSA_HASH, 5 },
    _oidSha1[]                      = { OID_OIW_ALGORITHM, 26 },
    _oidSha1DsaOIW[]                = { OID_OIW_ALGORITHM, 27 },
    _oidSha1DsaCommonOIW[]          = { OID_OIW_ALGORITHM, 28 },
    _oidSha1RsaOIW[]                = { OID_OIW_ALGORITHM, 29 },
    _oidSha256[]                    = { OID_NIST_HASHALG, 1 },
    _oidSha384[]                    = { OID_NIST_HASHALG, 2 },
    _oidSha512[]                    = { OID_NIST_HASHALG, 3 },
    _oidSha224[]                    = { OID_NIST_HASHALG, 4 },
    _oidFee[]                       = { APPLE_ALG_OID, 1 },
    _oidMd5Fee[]                    = { APPLE_ALG_OID, 3 },
    _oidSha1Fee[]                   = { APPLE_ALG_OID, 4 },
    _oidEcPrime192v1[]              = { OID_EC_CURVE, 1 },
    _oidEcPrime256v1[]              = { OID_EC_CURVE, 7 },
    _oidAnsip384r1[]                = { OID_CERTICOM_EC_CURVE, 34 },
    _oidAnsip521r1[]                = { OID_CERTICOM_EC_CURVE, 35 };

const DERItem
    oidRsa                          = { (DERByte *)_oidRsa,
                                        sizeof(_oidRsa) },
    oidMd2Rsa                       = { (DERByte *)_oidMd2Rsa,
                                        sizeof(_oidMd2Rsa) },
    oidMd4Rsa                       = { (DERByte *)_oidMd4Rsa,
                                        sizeof(_oidMd4Rsa) },
    oidMd5Rsa                       = { (DERByte *)_oidMd5Rsa,
                                        sizeof(_oidMd5Rsa) },
    oidSha1Rsa                      = { (DERByte *)_oidSha1Rsa,
                                        sizeof(_oidSha1Rsa) },
    oidSha256Rsa                    = { (DERByte *)_oidSha256Rsa,
                                        sizeof(_oidSha256Rsa) },
    oidSha384Rsa                    = { (DERByte *)_oidSha384Rsa,
                                        sizeof(_oidSha384Rsa) },
    oidSha512Rsa                    = { (DERByte *)_oidSha512Rsa,
                                        sizeof(_oidSha512Rsa) },
    oidSha224Rsa                    = { (DERByte *)_oidSha224Rsa,
                                        sizeof(_oidSha224Rsa) },
    oidEcPubKey                     = { (DERByte *)_oidEcPubKey,
                                        sizeof(_oidEcPubKey) },
    oidSha1Ecdsa                    = { (DERByte *)_oidSha1Ecdsa,
                                        sizeof(_oidSha1Ecdsa) },
    oidSha224Ecdsa                    = { (DERByte *)_oidSha224Ecdsa,
                                        sizeof(_oidSha224Ecdsa) },
    oidSha256Ecdsa                    = { (DERByte *)_oidSha256Ecdsa,
                                        sizeof(_oidSha256Ecdsa) },
    oidSha384Ecdsa                    = { (DERByte *)_oidSha384Ecdsa,
                                        sizeof(_oidSha384Ecdsa) },
    oidSha512Ecdsa                    = { (DERByte *)_oidSha512Ecdsa,
                                        sizeof(_oidSha512Ecdsa) },
    oidSha1Dsa                      = { (DERByte *)_oidSha1Dsa,
                                        sizeof(_oidSha1Dsa) },
    oidMd2                          = { (DERByte *)_oidMd2,
                                        sizeof(_oidMd2) },
    oidMd4                          = { (DERByte *)_oidMd4,
                                        sizeof(_oidMd4) },
    oidMd5                          = { (DERByte *)_oidMd5,
                                        sizeof(_oidMd5) },
    oidSha1                         = { (DERByte *)_oidSha1,
                                        sizeof(_oidSha1) },
    oidSha1RsaOIW                   = { (DERByte *)_oidSha1RsaOIW,
                                        sizeof(_oidSha1RsaOIW) },
    oidSha1DsaOIW                   = { (DERByte *)_oidSha1DsaOIW,
                                        sizeof(_oidSha1DsaOIW) },
    oidSha1DsaCommonOIW             = { (DERByte *)_oidSha1DsaCommonOIW,
                                        sizeof(_oidSha1DsaCommonOIW) },
    oidSha256                       = { (DERByte *)_oidSha256,
                                        sizeof(_oidSha256) },
    oidSha384                       = { (DERByte *)_oidSha384,
                                        sizeof(_oidSha384) },
    oidSha512                       = { (DERByte *)_oidSha512,
                                        sizeof(_oidSha512) },
    oidSha224                       = { (DERByte *)_oidSha224,
                                        sizeof(_oidSha224) },
    oidFee                          = { (DERByte *)_oidFee,
                                        sizeof(_oidFee) },
    oidMd5Fee                       = { (DERByte *)_oidMd5Fee,
                                        sizeof(_oidMd5Fee) },
    oidSha1Fee                      = { (DERByte *)_oidSha1Fee,
                                        sizeof(_oidSha1Fee) },
    oidEcPrime192v1                 = { (DERByte *)_oidEcPrime192v1,
                                        sizeof(_oidEcPrime192v1) },
    oidEcPrime256v1                 = { (DERByte *)_oidEcPrime256v1,
                                        sizeof(_oidEcPrime256v1) },
    oidAnsip384r1                   = { (DERByte *)_oidAnsip384r1,
                                        sizeof(_oidAnsip384r1) },
    oidAnsip521r1                   = { (DERByte *)_oidAnsip521r1,
                                        sizeof(_oidAnsip521r1) };


/* Extension OIDs. */
__unused static const DERByte
    _oidSubjectKeyIdentifier[]      = { OID_EXTENSION, 14 },
    _oidKeyUsage[]                  = { OID_EXTENSION, 15 },
    _oidPrivateKeyUsagePeriod[]     = { OID_EXTENSION, 16 },
    _oidSubjectAltName[]            = { OID_EXTENSION, 17 },
    _oidIssuerAltName[]             = { OID_EXTENSION, 18 },
    _oidBasicConstraints[]          = { OID_EXTENSION, 19 },
    _oidNameConstraints[]           = { OID_EXTENSION, 30 },
    _oidCrlDistributionPoints[]     = { OID_EXTENSION, 31 },
    _oidCertificatePolicies[]       = { OID_EXTENSION, 32 },
    _oidAnyPolicy[]                 = { OID_EXTENSION, 32, 0 },
    _oidPolicyMappings[]            = { OID_EXTENSION, 33 },
    _oidAuthorityKeyIdentifier[]    = { OID_EXTENSION, 35 },
    _oidPolicyConstraints[]         = { OID_EXTENSION, 36 },
    _oidExtendedKeyUsage[]          = { OID_EXTENSION, 37 },
    _oidAnyExtendedKeyUsage[]          = { OID_EXTENSION, 37, 0 },
    _oidInhibitAnyPolicy[]          = { OID_EXTENSION, 54 },
    _oidAuthorityInfoAccess[]       = { OID_PE, 1 },
    _oidSubjectInfoAccess[]         = { OID_PE, 11 },
    _oidAdOCSP[]                    = { OID_AD_OCSP },
    _oidAdCAIssuer[]                = { OID_AD_CAISSUERS },
    _oidNetscapeCertType[]          = { NETSCAPE_CERT_EXTEN, 1 },
    _oidEntrustVersInfo[]           = { ENTRUST_CERT_EXTEN, 0 },
    _oidMSNTPrincipalName[]         = { MICROSOFT_ENROLLMENT_OID, 2, 3 },
    /* Policy Qualifier IDs for Internet policy qualifiers. */
    _oidQtCps[]                     = { OID_QT, 1 },
    _oidQtUNotice[]                 = { OID_QT, 2 },
    /* X.501 Name IDs. */
    _oidCommonName[]                = { OID_ATTR_TYPE, 3 },
    _oidCountryName[]               = { OID_ATTR_TYPE, 6 },
    _oidLocalityName[]              = { OID_ATTR_TYPE, 7 },
    _oidStateOrProvinceName[]       = { OID_ATTR_TYPE, 8 },
    _oidOrganizationName[]          = { OID_ATTR_TYPE, 10 },
    _oidOrganizationalUnitName[]    = { OID_ATTR_TYPE, 11 },
    _oidDescription[]               = { OID_ATTR_TYPE, 13 },
    _oidEmailAddress[]              = { OID_PKCS_9, 1 },
    _oidFriendlyName[]              = { OID_PKCS_9, 20 },
    _oidLocalKeyId[]                = { OID_PKCS_9, 21 },
    _oidExtendedKeyUsageServerAuth[] = { OID_KP, 1 },
    _oidExtendedKeyUsageClientAuth[] = { OID_KP, 2 },
    _oidExtendedKeyUsageCodeSigning[] = { OID_KP, 3 },
    _oidExtendedKeyUsageEmailProtection[] = { OID_KP, 4 },
    _oidExtendedKeyUsageTimeStamping[] = { OID_KP, 8 },
    _oidExtendedKeyUsageOCSPSigning[] = { OID_KP, 9 },
    _oidExtendedKeyUsageIPSec[]     = { OID_ISAKMP, 2, 2 },
    _oidExtendedKeyUsageMicrosoftSGC[] = { MICROSOFT_BASE_OID, 10, 3, 3 },
    _oidExtendedKeyUsageNetscapeSGC[] = { NETSCAPE_CERT_POLICY, 1 },
    _oidGoogleEmbeddedSignedCertificateTimestamp[] = {GOOGLE_EMBEDDED_SCT_OID},
    _oidGoogleOCSPSignedCertificateTimestamp[] = {GOOGLE_OCSP_SCT_OID};

__unused const DERItem
    oidSubjectKeyIdentifier         = { (DERByte *)_oidSubjectKeyIdentifier,
                                        sizeof(_oidSubjectKeyIdentifier) },
    oidKeyUsage                     = { (DERByte *)_oidKeyUsage,
                                        sizeof(_oidKeyUsage) },
    oidPrivateKeyUsagePeriod        = { (DERByte *)_oidPrivateKeyUsagePeriod,
                                        sizeof(_oidPrivateKeyUsagePeriod) },
    oidSubjectAltName               = { (DERByte *)_oidSubjectAltName,
                                        sizeof(_oidSubjectAltName) },
    oidIssuerAltName                = { (DERByte *)_oidIssuerAltName,
                                        sizeof(_oidIssuerAltName) },
    oidBasicConstraints             = { (DERByte *)_oidBasicConstraints,
                                        sizeof(_oidBasicConstraints) },
    oidNameConstraints              = { (DERByte *)_oidNameConstraints,
                                        sizeof(_oidNameConstraints) },
    oidCrlDistributionPoints        = { (DERByte *)_oidCrlDistributionPoints,
                                        sizeof(_oidCrlDistributionPoints) },
    oidCertificatePolicies          = { (DERByte *)_oidCertificatePolicies,
                                        sizeof(_oidCertificatePolicies) },
    oidAnyPolicy                    = { (DERByte *)_oidAnyPolicy,
                                        sizeof(_oidAnyPolicy) },
    oidPolicyMappings               = { (DERByte *)_oidPolicyMappings,
                                        sizeof(_oidPolicyMappings) },
    oidAuthorityKeyIdentifier       = { (DERByte *)_oidAuthorityKeyIdentifier,
                                        sizeof(_oidAuthorityKeyIdentifier) },
    oidPolicyConstraints            = { (DERByte *)_oidPolicyConstraints,
                                        sizeof(_oidPolicyConstraints) },
    oidExtendedKeyUsage             = { (DERByte *)_oidExtendedKeyUsage,
                                        sizeof(_oidExtendedKeyUsage) },
    oidAnyExtendedKeyUsage          = { (DERByte *)_oidAnyExtendedKeyUsage,
                                        sizeof(_oidAnyExtendedKeyUsage) },
    oidInhibitAnyPolicy             = { (DERByte *)_oidInhibitAnyPolicy,
                                        sizeof(_oidInhibitAnyPolicy) },
    oidAuthorityInfoAccess          = { (DERByte *)_oidAuthorityInfoAccess,
                                        sizeof(_oidAuthorityInfoAccess) },
    oidSubjectInfoAccess            = { (DERByte *)_oidSubjectInfoAccess,
                                        sizeof(_oidSubjectInfoAccess) },
    oidAdOCSP                       = { (DERByte *)_oidAdOCSP,
                                        sizeof(_oidAdOCSP) },
    oidAdCAIssuer                   = { (DERByte *)_oidAdCAIssuer,
                                        sizeof(_oidAdCAIssuer) },
    oidNetscapeCertType             = { (DERByte *)_oidNetscapeCertType,
                                        sizeof(_oidNetscapeCertType) },
    oidEntrustVersInfo              = { (DERByte *)_oidEntrustVersInfo,
                                        sizeof(_oidEntrustVersInfo) },
    oidMSNTPrincipalName              = { (DERByte *)_oidMSNTPrincipalName,
                                        sizeof(_oidMSNTPrincipalName) },
    /* Policy Qualifier IDs for Internet policy qualifiers. */
    oidQtCps                        = { (DERByte *)_oidQtCps,
                                        sizeof(_oidQtCps) },
    oidQtUNotice                    = { (DERByte *)_oidQtUNotice,
                                        sizeof(_oidQtUNotice) },
    /* X.501 Name IDs. */
    oidCommonName                   = { (DERByte *)_oidCommonName,
                                        sizeof(_oidCommonName) },
    oidCountryName                  = { (DERByte *)_oidCountryName,
                                        sizeof(_oidCountryName) },
    oidLocalityName                 = { (DERByte *)_oidLocalityName,
                                        sizeof(_oidLocalityName) },
    oidStateOrProvinceName          = { (DERByte *)_oidStateOrProvinceName,
                                        sizeof(_oidStateOrProvinceName) },
    oidOrganizationName             = { (DERByte *)_oidOrganizationName,
                                        sizeof(_oidOrganizationName) },
    oidOrganizationalUnitName       = { (DERByte *)_oidOrganizationalUnitName,
                                        sizeof(_oidOrganizationalUnitName) },
    oidDescription                  = { (DERByte *)_oidDescription,
                                        sizeof(_oidDescription) },
    oidEmailAddress                 = { (DERByte *)_oidEmailAddress,
                                        sizeof(_oidEmailAddress) },
    oidFriendlyName                 = { (DERByte *)_oidFriendlyName,
                                        sizeof(_oidFriendlyName) },
    oidLocalKeyId                   = { (DERByte *)_oidLocalKeyId,
                                        sizeof(_oidLocalKeyId) },
    oidExtendedKeyUsageServerAuth   = { (DERByte *)_oidExtendedKeyUsageServerAuth,
                                        sizeof(_oidExtendedKeyUsageServerAuth) },
    oidExtendedKeyUsageClientAuth   = { (DERByte *)_oidExtendedKeyUsageClientAuth,
                                        sizeof(_oidExtendedKeyUsageClientAuth) },
    oidExtendedKeyUsageCodeSigning  = { (DERByte *)_oidExtendedKeyUsageCodeSigning,
                                        sizeof(_oidExtendedKeyUsageCodeSigning) },
    oidExtendedKeyUsageEmailProtection  = { (DERByte *)_oidExtendedKeyUsageEmailProtection,
                                        sizeof(_oidExtendedKeyUsageEmailProtection) },
    oidExtendedKeyUsageTimeStamping = { (DERByte *)_oidExtendedKeyUsageTimeStamping,
                                        sizeof(_oidExtendedKeyUsageTimeStamping) },
    oidExtendedKeyUsageOCSPSigning  = { (DERByte *)_oidExtendedKeyUsageOCSPSigning,
                                        sizeof(_oidExtendedKeyUsageOCSPSigning) },
    oidExtendedKeyUsageIPSec        = { (DERByte *)_oidExtendedKeyUsageIPSec,
                                        sizeof(_oidExtendedKeyUsageIPSec) },
    oidExtendedKeyUsageMicrosoftSGC = { (DERByte *)_oidExtendedKeyUsageMicrosoftSGC,
                                        sizeof(_oidExtendedKeyUsageMicrosoftSGC) },
    oidExtendedKeyUsageNetscapeSGC  = { (DERByte *)_oidExtendedKeyUsageNetscapeSGC,
                                        sizeof(_oidExtendedKeyUsageNetscapeSGC) },
    oidGoogleEmbeddedSignedCertificateTimestamp
                                      = { (DERByte *)_oidGoogleEmbeddedSignedCertificateTimestamp,
                                        sizeof(_oidGoogleEmbeddedSignedCertificateTimestamp) },
    oidGoogleOCSPSignedCertificateTimestamp
                                      = { (DERByte *)_oidGoogleOCSPSignedCertificateTimestamp,
                                        sizeof(_oidGoogleOCSPSignedCertificateTimestamp) };



