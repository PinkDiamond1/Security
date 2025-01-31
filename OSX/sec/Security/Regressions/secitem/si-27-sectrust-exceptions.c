/*
 * Copyright (c) 2006-2010,2012-2018 Apple Inc. All Rights Reserved.
 */

#include <CoreFoundation/CoreFoundation.h>
#include <Security/SecCertificate.h>
#include <Security/SecCertificatePriv.h>
#include <Security/SecPolicy.h>
#include <Security/SecTrustPriv.h>
#include <utilities/array_size.h>
#include <stdlib.h>
#include <unistd.h>

#include "shared_regressions.h"

/*
 Serial Number:
 01:f1:eb:db:ee:d3:1d:7a:b5:72:54:7d:34:43:4b:87
 Signature Algorithm: sha256WithRSAEncryption
 Issuer: C=US, O=DigiCert Inc, OU=www.digicert.com, CN=DigiCert SHA2 Extended Validation Server CA
 Validity
 Not Before: Mar 22 00:00:00 2018 GMT
 Not After : Mar 23 12:00:00 2019 GMT
 Subject: businessCategory=Private Organization/jurisdictionC=US/jurisdictionST=California/serialNumber=C0806592, C=US, ST=California, L=Cupertino, O=Apple Inc., OU=Internet Services for Akamai, CN=store.apple.com
 */
static unsigned char _c0[]={
    0x30,0x82,0x06,0xF7,0x30,0x82,0x05,0xDF,0xA0,0x03,0x02,0x01,0x02,0x02,0x10,0x01,
    0xF1,0xEB,0xDB,0xEE,0xD3,0x1D,0x7A,0xB5,0x72,0x54,0x7D,0x34,0x43,0x4B,0x87,0x30,
    0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x75,
    0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x15,0x30,
    0x13,0x06,0x03,0x55,0x04,0x0A,0x13,0x0C,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,
    0x20,0x49,0x6E,0x63,0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0B,0x13,0x10,0x77,
    0x77,0x77,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x31,
    0x34,0x30,0x32,0x06,0x03,0x55,0x04,0x03,0x13,0x2B,0x44,0x69,0x67,0x69,0x43,0x65,
    0x72,0x74,0x20,0x53,0x48,0x41,0x32,0x20,0x45,0x78,0x74,0x65,0x6E,0x64,0x65,0x64,
    0x20,0x56,0x61,0x6C,0x69,0x64,0x61,0x74,0x69,0x6F,0x6E,0x20,0x53,0x65,0x72,0x76,
    0x65,0x72,0x20,0x43,0x41,0x30,0x1E,0x17,0x0D,0x31,0x38,0x30,0x33,0x32,0x32,0x30,
    0x30,0x30,0x30,0x30,0x30,0x5A,0x17,0x0D,0x31,0x39,0x30,0x33,0x32,0x33,0x31,0x32,
    0x30,0x30,0x30,0x30,0x5A,0x30,0x81,0xF0,0x31,0x1D,0x30,0x1B,0x06,0x03,0x55,0x04,
    0x0F,0x0C,0x14,0x50,0x72,0x69,0x76,0x61,0x74,0x65,0x20,0x4F,0x72,0x67,0x61,0x6E,
    0x69,0x7A,0x61,0x74,0x69,0x6F,0x6E,0x31,0x13,0x30,0x11,0x06,0x0B,0x2B,0x06,0x01,
    0x04,0x01,0x82,0x37,0x3C,0x02,0x01,0x03,0x13,0x02,0x55,0x53,0x31,0x1B,0x30,0x19,
    0x06,0x0B,0x2B,0x06,0x01,0x04,0x01,0x82,0x37,0x3C,0x02,0x01,0x02,0x13,0x0A,0x43,
    0x61,0x6C,0x69,0x66,0x6F,0x72,0x6E,0x69,0x61,0x31,0x11,0x30,0x0F,0x06,0x03,0x55,
    0x04,0x05,0x13,0x08,0x43,0x30,0x38,0x30,0x36,0x35,0x39,0x32,0x31,0x0B,0x30,0x09,
    0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x13,0x30,0x11,0x06,0x03,0x55,
    0x04,0x08,0x13,0x0A,0x43,0x61,0x6C,0x69,0x66,0x6F,0x72,0x6E,0x69,0x61,0x31,0x12,
    0x30,0x10,0x06,0x03,0x55,0x04,0x07,0x13,0x09,0x43,0x75,0x70,0x65,0x72,0x74,0x69,
    0x6E,0x6F,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x13,0x0A,0x41,0x70,0x70,
    0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x25,0x30,0x23,0x06,0x03,0x55,0x04,0x0B,
    0x13,0x1C,0x49,0x6E,0x74,0x65,0x72,0x6E,0x65,0x74,0x20,0x53,0x65,0x72,0x76,0x69,
    0x63,0x65,0x73,0x20,0x66,0x6F,0x72,0x20,0x41,0x6B,0x61,0x6D,0x61,0x69,0x31,0x18,
    0x30,0x16,0x06,0x03,0x55,0x04,0x03,0x13,0x0F,0x73,0x74,0x6F,0x72,0x65,0x2E,0x61,
    0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,
    0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,
    0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xB5,0xDF,0x48,0x6A,0xE5,0x5F,0x0C,
    0x42,0xBF,0x71,0x07,0xAB,0xE6,0x48,0xD2,0x2C,0x13,0x7C,0x29,0xF7,0x90,0xC2,0x45,
    0x39,0x43,0x48,0x2E,0x16,0x22,0xBF,0xAB,0x48,0x20,0x14,0x27,0xE7,0x87,0x52,0x97,
    0xF6,0x64,0x0C,0x62,0xF5,0x39,0x54,0x64,0x09,0xE8,0x29,0x56,0xF4,0x03,0x40,0x5A,
    0xFA,0x9A,0x11,0x2B,0xD0,0x01,0x51,0x1E,0xED,0xB8,0x51,0xCB,0xAB,0x6B,0x7A,0x99,
    0xF3,0xB0,0x6E,0xA8,0xC4,0xB6,0xAF,0x17,0xEC,0xA6,0xD2,0x4D,0xCA,0x63,0x3D,0x59,
    0x30,0x25,0x30,0x54,0x86,0xDB,0x70,0x7A,0xEC,0x07,0x83,0x19,0xA0,0x44,0xE7,0x3C,
    0x68,0xE0,0xFD,0xB9,0xEE,0x3F,0xAC,0xF3,0x32,0x5A,0x5F,0x42,0x31,0x94,0x70,0x2C,
    0xC5,0x73,0xD2,0x79,0x23,0x5B,0x96,0x45,0xB1,0xB3,0x2A,0xA1,0x5A,0x69,0xFE,0xBE,
    0x52,0x0D,0x5D,0x79,0x18,0xCA,0xF1,0x44,0x92,0xA0,0x27,0x1F,0xAA,0x6E,0x9D,0x6F,
    0x1B,0x83,0x5B,0x73,0x28,0x1D,0x87,0xB5,0x70,0x0E,0x3D,0xED,0xE2,0xC2,0x34,0x8A,
    0x81,0xB2,0x22,0x40,0x98,0x77,0x2F,0x34,0x1B,0x70,0xEC,0x96,0x3F,0x91,0xB9,0xFF,
    0xC9,0xE5,0x7E,0xE7,0x25,0xEF,0xDB,0x9A,0x58,0x4E,0xB2,0x92,0x19,0xA5,0x8D,0xEB,
    0x76,0xF8,0xA8,0x48,0x9F,0x3D,0x10,0x0C,0xE4,0x69,0x7B,0xE7,0xB7,0xCA,0xF6,0x14,
    0x5E,0x93,0x1E,0x20,0x37,0x1B,0xB8,0xB1,0x2C,0xD1,0x46,0x5C,0xD7,0x85,0x4A,0x2E,
    0x19,0xB2,0x3C,0x31,0xDC,0x3D,0xB5,0x3C,0xEC,0x49,0x8D,0x9C,0xCF,0x75,0x0B,0xFC,
    0x03,0x31,0x37,0xE7,0xA5,0xD6,0x9D,0x19,0x0D,0x02,0x03,0x01,0x00,0x01,0xA3,0x82,
    0x03,0x05,0x30,0x82,0x03,0x01,0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,
    0x16,0x80,0x14,0x3D,0xD3,0x50,0xA5,0xD6,0xA0,0xAD,0xEE,0xF3,0x4A,0x60,0x0A,0x65,
    0xD3,0x21,0xD4,0xF8,0xF8,0xD6,0x0F,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,
    0x04,0x14,0x6F,0x89,0xAE,0x9B,0xE4,0x8A,0x21,0x3A,0x3B,0xEA,0xAA,0xBE,0x99,0x90,
    0xC8,0xDB,0x34,0x80,0x21,0xB9,0x30,0x2F,0x06,0x03,0x55,0x1D,0x11,0x04,0x28,0x30,
    0x26,0x82,0x0F,0x73,0x74,0x6F,0x72,0x65,0x2E,0x61,0x70,0x70,0x6C,0x65,0x2E,0x63,
    0x6F,0x6D,0x82,0x13,0x77,0x77,0x77,0x2E,0x73,0x74,0x6F,0x72,0x65,0x2E,0x61,0x70,
    0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,
    0xFF,0x04,0x04,0x03,0x02,0x05,0xA0,0x30,0x1D,0x06,0x03,0x55,0x1D,0x25,0x04,0x16,
    0x30,0x14,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x03,0x01,0x06,0x08,0x2B,0x06,
    0x01,0x05,0x05,0x07,0x03,0x02,0x30,0x75,0x06,0x03,0x55,0x1D,0x1F,0x04,0x6E,0x30,
    0x6C,0x30,0x34,0xA0,0x32,0xA0,0x30,0x86,0x2E,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,
    0x63,0x72,0x6C,0x33,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,
    0x6D,0x2F,0x73,0x68,0x61,0x32,0x2D,0x65,0x76,0x2D,0x73,0x65,0x72,0x76,0x65,0x72,
    0x2D,0x67,0x32,0x2E,0x63,0x72,0x6C,0x30,0x34,0xA0,0x32,0xA0,0x30,0x86,0x2E,0x68,
    0x74,0x74,0x70,0x3A,0x2F,0x2F,0x63,0x72,0x6C,0x34,0x2E,0x64,0x69,0x67,0x69,0x63,
    0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x2F,0x73,0x68,0x61,0x32,0x2D,0x65,0x76,0x2D,
    0x73,0x65,0x72,0x76,0x65,0x72,0x2D,0x67,0x32,0x2E,0x63,0x72,0x6C,0x30,0x4B,0x06,
    0x03,0x55,0x1D,0x20,0x04,0x44,0x30,0x42,0x30,0x37,0x06,0x09,0x60,0x86,0x48,0x01,
    0x86,0xFD,0x6C,0x02,0x01,0x30,0x2A,0x30,0x28,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,
    0x07,0x02,0x01,0x16,0x1C,0x68,0x74,0x74,0x70,0x73,0x3A,0x2F,0x2F,0x77,0x77,0x77,
    0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x2F,0x43,0x50,
    0x53,0x30,0x07,0x06,0x05,0x67,0x81,0x0C,0x01,0x01,0x30,0x81,0x88,0x06,0x08,0x2B,
    0x06,0x01,0x05,0x05,0x07,0x01,0x01,0x04,0x7C,0x30,0x7A,0x30,0x24,0x06,0x08,0x2B,
    0x06,0x01,0x05,0x05,0x07,0x30,0x01,0x86,0x18,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,
    0x6F,0x63,0x73,0x70,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,
    0x6D,0x30,0x52,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x30,0x02,0x86,0x46,0x68,
    0x74,0x74,0x70,0x3A,0x2F,0x2F,0x63,0x61,0x63,0x65,0x72,0x74,0x73,0x2E,0x64,0x69,
    0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x2F,0x44,0x69,0x67,0x69,0x43,
    0x65,0x72,0x74,0x53,0x48,0x41,0x32,0x45,0x78,0x74,0x65,0x6E,0x64,0x65,0x64,0x56,
    0x61,0x6C,0x69,0x64,0x61,0x74,0x69,0x6F,0x6E,0x53,0x65,0x72,0x76,0x65,0x72,0x43,
    0x41,0x2E,0x63,0x72,0x74,0x30,0x09,0x06,0x03,0x55,0x1D,0x13,0x04,0x02,0x30,0x00,
    0x30,0x82,0x01,0x03,0x06,0x0A,0x2B,0x06,0x01,0x04,0x01,0xD6,0x79,0x02,0x04,0x02,
    0x04,0x81,0xF4,0x04,0x81,0xF1,0x00,0xEF,0x00,0x75,0x00,0xA4,0xB9,0x09,0x90,0xB4,
    0x18,0x58,0x14,0x87,0xBB,0x13,0xA2,0xCC,0x67,0x70,0x0A,0x3C,0x35,0x98,0x04,0xF9,
    0x1B,0xDF,0xB8,0xE3,0x77,0xCD,0x0E,0xC8,0x0D,0xDC,0x10,0x00,0x00,0x01,0x62,0x4E,
    0xBC,0xC7,0x43,0x00,0x00,0x04,0x03,0x00,0x46,0x30,0x44,0x02,0x20,0x3E,0xD3,0xFE,
    0xB4,0x45,0x97,0xCE,0xA3,0x05,0xC9,0x29,0x70,0x55,0x3B,0x77,0x9E,0xCC,0x4B,0x06,
    0xFD,0x76,0x21,0xD0,0x79,0x69,0xD6,0x60,0x01,0xBB,0xC7,0x43,0x5E,0x02,0x20,0x3D,
    0xB7,0x73,0x91,0x51,0xB7,0xAC,0x40,0xFB,0xA7,0x36,0xCF,0x10,0xE8,0x63,0x79,0xE6,
    0x06,0xEC,0xFA,0x60,0xFE,0x44,0x90,0x9A,0x53,0x26,0x04,0x27,0x1A,0x4B,0xD4,0x00,
    0x76,0x00,0x56,0x14,0x06,0x9A,0x2F,0xD7,0xC2,0xEC,0xD3,0xF5,0xE1,0xBD,0x44,0xB2,
    0x3E,0xC7,0x46,0x76,0xB9,0xBC,0x99,0x11,0x5C,0xC0,0xEF,0x94,0x98,0x55,0xD6,0x89,
    0xD0,0xDD,0x00,0x00,0x01,0x62,0x4E,0xBC,0xC8,0x6A,0x00,0x00,0x04,0x03,0x00,0x47,
    0x30,0x45,0x02,0x20,0x18,0xF4,0x40,0x99,0x83,0xA7,0x53,0x6D,0x11,0xBF,0x7C,0xA5,
    0x64,0x21,0x52,0xD0,0x7C,0xF5,0x96,0xCD,0xB0,0x56,0xAE,0x1E,0x13,0x9C,0xFC,0x08,
    0x3B,0x56,0x66,0x0F,0x02,0x21,0x00,0x98,0xBE,0xC7,0x01,0x8E,0x88,0xB7,0xB2,0xF9,
    0xC7,0xE6,0x08,0x46,0x10,0xEA,0x8D,0x01,0x12,0x0D,0x7D,0xA6,0xBA,0xA6,0xD3,0x1F,
    0xEC,0xA3,0xD6,0x7A,0xE4,0x85,0x89,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x6A,0x52,0xE8,0xB3,0x70,
    0x25,0xD9,0x95,0x87,0xC4,0xF3,0x14,0xB1,0x8C,0x29,0x6F,0x11,0xA0,0x6D,0xD7,0xE9,
    0x96,0xFA,0x06,0x57,0x4D,0x86,0xE8,0xD6,0x95,0xC0,0x3A,0x33,0xEC,0x67,0x6C,0x98,
    0xA2,0x99,0x09,0x28,0x16,0x18,0xEC,0x21,0xED,0x93,0xEF,0xAB,0x69,0x5F,0x98,0x9C,
    0x62,0x21,0x10,0xEB,0x1C,0xC4,0x20,0x5E,0x3E,0x6F,0x80,0x03,0xC9,0xC5,0xD5,0x2F,
    0xDA,0x8C,0x86,0x40,0x59,0x71,0x34,0xEE,0xCF,0x54,0x83,0xE1,0x3E,0x11,0x1D,0x12,
    0x66,0x3D,0x16,0x7F,0xD8,0x1B,0x42,0x35,0x50,0xA6,0xD6,0xE6,0x6B,0x32,0xDE,0xE2,
    0xD9,0x01,0xD7,0xB1,0xF5,0xE8,0x72,0x44,0x03,0xB5,0xFA,0x19,0x0D,0xF2,0x8D,0x0B,
    0x75,0xBA,0xD9,0x39,0xFF,0x73,0xF2,0xA0,0xD6,0x49,0xEF,0x9E,0xE9,0x23,0x5D,0xED,
    0xC5,0x08,0x69,0x10,0xF7,0xF0,0x0B,0x8D,0x80,0xB6,0x43,0xE4,0x2A,0xEC,0x92,0xCF,
    0x22,0xCA,0xAF,0x1A,0x8A,0x16,0xC7,0xC5,0x88,0xB7,0xCA,0xC6,0x19,0x1A,0xD1,0xFF,
    0x13,0x93,0x56,0x29,0x1A,0x48,0xA8,0x92,0x21,0xE5,0x7F,0x3E,0x4C,0xB4,0x89,0xD6,
    0x08,0xF0,0xB0,0x4B,0x22,0x7C,0x1F,0xEC,0xE7,0x09,0x5D,0x23,0xE2,0xD1,0xB8,0xA3,
    0xDF,0xEC,0x2D,0x87,0x1F,0xCD,0x58,0x1C,0xDD,0x09,0xE8,0xB5,0xD4,0x30,0x1E,0x2A,
    0x4D,0xA3,0xA1,0x84,0x43,0xD7,0x4A,0x7B,0x15,0x8E,0xEB,0xE2,0x53,0x2B,0x12,0xCB,
    0xFB,0xF2,0x61,0x7E,0x92,0x0B,0x87,0x07,0x1C,0x8D,0x0A,0xED,0x62,0x10,0x27,0xE3,
    0xDB,0xC4,0x65,0xDE,0x57,0xFB,0x6D,0x49,0xC8,0x7A,0xF8,
};

/* subject:/C=US/O=DigiCert Inc/OU=www.digicert.com/CN=DigiCert SHA2 Extended Validation Server CA */
/* issuer :/C=US/O=DigiCert Inc/OU=www.digicert.com/CN=DigiCert High Assurance EV Root CA */
static unsigned char _c1[]={
    0x30,0x82,0x04,0xB6,0x30,0x82,0x03,0x9E,0xA0,0x03,0x02,0x01,0x02,0x02,0x10,0x0C,
    0x79,0xA9,0x44,0xB0,0x8C,0x11,0x95,0x20,0x92,0x61,0x5F,0xE2,0x6B,0x1D,0x83,0x30,
    0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x6C,
    0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x15,0x30,
    0x13,0x06,0x03,0x55,0x04,0x0A,0x13,0x0C,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,
    0x20,0x49,0x6E,0x63,0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0B,0x13,0x10,0x77,
    0x77,0x77,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x31,
    0x2B,0x30,0x29,0x06,0x03,0x55,0x04,0x03,0x13,0x22,0x44,0x69,0x67,0x69,0x43,0x65,
    0x72,0x74,0x20,0x48,0x69,0x67,0x68,0x20,0x41,0x73,0x73,0x75,0x72,0x61,0x6E,0x63,
    0x65,0x20,0x45,0x56,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x30,0x1E,0x17,0x0D,
    0x31,0x33,0x31,0x30,0x32,0x32,0x31,0x32,0x30,0x30,0x30,0x30,0x5A,0x17,0x0D,0x32,
    0x38,0x31,0x30,0x32,0x32,0x31,0x32,0x30,0x30,0x30,0x30,0x5A,0x30,0x75,0x31,0x0B,
    0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x15,0x30,0x13,0x06,
    0x03,0x55,0x04,0x0A,0x13,0x0C,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x20,0x49,
    0x6E,0x63,0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0B,0x13,0x10,0x77,0x77,0x77,
    0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x31,0x34,0x30,
    0x32,0x06,0x03,0x55,0x04,0x03,0x13,0x2B,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,
    0x20,0x53,0x48,0x41,0x32,0x20,0x45,0x78,0x74,0x65,0x6E,0x64,0x65,0x64,0x20,0x56,
    0x61,0x6C,0x69,0x64,0x61,0x74,0x69,0x6F,0x6E,0x20,0x53,0x65,0x72,0x76,0x65,0x72,
    0x20,0x43,0x41,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,
    0x82,0x01,0x01,0x00,0xD7,0x53,0xA4,0x04,0x51,0xF8,0x99,0xA6,0x16,0x48,0x4B,0x67,
    0x27,0xAA,0x93,0x49,0xD0,0x39,0xED,0x0C,0xB0,0xB0,0x00,0x87,0xF1,0x67,0x28,0x86,
    0x85,0x8C,0x8E,0x63,0xDA,0xBC,0xB1,0x40,0x38,0xE2,0xD3,0xF5,0xEC,0xA5,0x05,0x18,
    0xB8,0x3D,0x3E,0xC5,0x99,0x17,0x32,0xEC,0x18,0x8C,0xFA,0xF1,0x0C,0xA6,0x64,0x21,
    0x85,0xCB,0x07,0x10,0x34,0xB0,0x52,0x88,0x2B,0x1F,0x68,0x9B,0xD2,0xB1,0x8F,0x12,
    0xB0,0xB3,0xD2,0xE7,0x88,0x1F,0x1F,0xEF,0x38,0x77,0x54,0x53,0x5F,0x80,0x79,0x3F,
    0x2E,0x1A,0xAA,0xA8,0x1E,0x4B,0x2B,0x0D,0xAB,0xB7,0x63,0xB9,0x35,0xB7,0x7D,0x14,
    0xBC,0x59,0x4B,0xDF,0x51,0x4A,0xD2,0xA1,0xE2,0x0C,0xE2,0x90,0x82,0x87,0x6A,0xAE,
    0xEA,0xD7,0x64,0xD6,0x98,0x55,0xE8,0xFD,0xAF,0x1A,0x50,0x6C,0x54,0xBC,0x11,0xF2,
    0xFD,0x4A,0xF2,0x9D,0xBB,0x7F,0x0E,0xF4,0xD5,0xBE,0x8E,0x16,0x89,0x12,0x55,0xD8,
    0xC0,0x71,0x34,0xEE,0xF6,0xDC,0x2D,0xEC,0xC4,0x87,0x25,0x86,0x8D,0xD8,0x21,0xE4,
    0xB0,0x4D,0x0C,0x89,0xDC,0x39,0x26,0x17,0xDD,0xF6,0xD7,0x94,0x85,0xD8,0x04,0x21,
    0x70,0x9D,0x6F,0x6F,0xFF,0x5C,0xBA,0x19,0xE1,0x45,0xCB,0x56,0x57,0x28,0x7E,0x1C,
    0x0D,0x41,0x57,0xAA,0xB7,0xB8,0x27,0xBB,0xB1,0xE4,0xFA,0x2A,0xEF,0x21,0x23,0x75,
    0x1A,0xAD,0x2D,0x9B,0x86,0x35,0x8C,0x9C,0x77,0xB5,0x73,0xAD,0xD8,0x94,0x2D,0xE4,
    0xF3,0x0C,0x9D,0xEE,0xC1,0x4E,0x62,0x7E,0x17,0xC0,0x71,0x9E,0x2C,0xDE,0xF1,0xF9,
    0x10,0x28,0x19,0x33,0x02,0x03,0x01,0x00,0x01,0xA3,0x82,0x01,0x49,0x30,0x82,0x01,
    0x45,0x30,0x12,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x08,0x30,0x06,0x01,
    0x01,0xFF,0x02,0x01,0x00,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,
    0x04,0x03,0x02,0x01,0x86,0x30,0x1D,0x06,0x03,0x55,0x1D,0x25,0x04,0x16,0x30,0x14,
    0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x03,0x01,0x06,0x08,0x2B,0x06,0x01,0x05,
    0x05,0x07,0x03,0x02,0x30,0x34,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x01,0x01,
    0x04,0x28,0x30,0x26,0x30,0x24,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x30,0x01,
    0x86,0x18,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x6F,0x63,0x73,0x70,0x2E,0x64,0x69,
    0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x30,0x4B,0x06,0x03,0x55,0x1D,
    0x1F,0x04,0x44,0x30,0x42,0x30,0x40,0xA0,0x3E,0xA0,0x3C,0x86,0x3A,0x68,0x74,0x74,
    0x70,0x3A,0x2F,0x2F,0x63,0x72,0x6C,0x34,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,
    0x74,0x2E,0x63,0x6F,0x6D,0x2F,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x48,0x69,
    0x67,0x68,0x41,0x73,0x73,0x75,0x72,0x61,0x6E,0x63,0x65,0x45,0x56,0x52,0x6F,0x6F,
    0x74,0x43,0x41,0x2E,0x63,0x72,0x6C,0x30,0x3D,0x06,0x03,0x55,0x1D,0x20,0x04,0x36,
    0x30,0x34,0x30,0x32,0x06,0x04,0x55,0x1D,0x20,0x00,0x30,0x2A,0x30,0x28,0x06,0x08,
    0x2B,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,0x1C,0x68,0x74,0x74,0x70,0x73,0x3A,
    0x2F,0x2F,0x77,0x77,0x77,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,
    0x6F,0x6D,0x2F,0x43,0x50,0x53,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,
    0x14,0x3D,0xD3,0x50,0xA5,0xD6,0xA0,0xAD,0xEE,0xF3,0x4A,0x60,0x0A,0x65,0xD3,0x21,
    0xD4,0xF8,0xF8,0xD6,0x0F,0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,
    0x80,0x14,0xB1,0x3E,0xC3,0x69,0x03,0xF8,0xBF,0x47,0x01,0xD4,0x98,0x26,0x1A,0x08,
    0x02,0xEF,0x63,0x64,0x2B,0xC3,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,
    0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x9D,0xB6,0xD0,0x90,0x86,0xE1,
    0x86,0x02,0xED,0xC5,0xA0,0xF0,0x34,0x1C,0x74,0xC1,0x8D,0x76,0xCC,0x86,0x0A,0xA8,
    0xF0,0x4A,0x8A,0x42,0xD6,0x3F,0xC8,0xA9,0x4D,0xAD,0x7C,0x08,0xAD,0xE6,0xB6,0x50,
    0xB8,0xA2,0x1A,0x4D,0x88,0x07,0xB1,0x29,0x21,0xDC,0xE7,0xDA,0xC6,0x3C,0x21,0xE0,
    0xE3,0x11,0x49,0x70,0xAC,0x7A,0x1D,0x01,0xA4,0xCA,0x11,0x3A,0x57,0xAB,0x7D,0x57,
    0x2A,0x40,0x74,0xFD,0xD3,0x1D,0x85,0x18,0x50,0xDF,0x57,0x47,0x75,0xA1,0x7D,0x55,
    0x20,0x2E,0x47,0x37,0x50,0x72,0x8C,0x7F,0x82,0x1B,0xD2,0x62,0x8F,0x2D,0x03,0x5A,
    0xDA,0xC3,0xC8,0xA1,0xCE,0x2C,0x52,0xA2,0x00,0x63,0xEB,0x73,0xBA,0x71,0xC8,0x49,
    0x27,0x23,0x97,0x64,0x85,0x9E,0x38,0x0E,0xAD,0x63,0x68,0x3C,0xBA,0x52,0x81,0x58,
    0x79,0xA3,0x2C,0x0C,0xDF,0xDE,0x6D,0xEB,0x31,0xF2,0xBA,0xA0,0x7C,0x6C,0xF1,0x2C,
    0xD4,0xE1,0xBD,0x77,0x84,0x37,0x03,0xCE,0x32,0xB5,0xC8,0x9A,0x81,0x1A,0x4A,0x92,
    0x4E,0x3B,0x46,0x9A,0x85,0xFE,0x83,0xA2,0xF9,0x9E,0x8C,0xA3,0xCC,0x0D,0x5E,0xB3,
    0x3D,0xCF,0x04,0x78,0x8F,0x14,0x14,0x7B,0x32,0x9C,0xC7,0x00,0xA6,0x5C,0xC4,0xB5,
    0xA1,0x55,0x8D,0x5A,0x56,0x68,0xA4,0x22,0x70,0xAA,0x3C,0x81,0x71,0xD9,0x9D,0xA8,
    0x45,0x3B,0xF4,0xE5,0xF6,0xA2,0x51,0xDD,0xC7,0x7B,0x62,0xE8,0x6F,0x0C,0x74,0xEB,
    0xB8,0xDA,0xF8,0xBF,0x87,0x0D,0x79,0x50,0x91,0x90,0x9B,0x18,0x3B,0x91,0x59,0x27,
    0xF1,0x35,0x28,0x13,0xAB,0x26,0x7E,0xD5,0xF7,0x7A,
};

/* subject:/CN=self-signed.ssltest.apple.com/C=US */
/* issuer :/CN=self-signed.ssltest.apple.com/C=US */

static unsigned char _ss0[]={
	0x30,0x82,0x03,0x0F,0x30,0x82,0x01,0xF7,0xA0,0x03,0x02,0x01,0x02,0x02,0x01,0x01,
	0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,
	0x35,0x31,0x26,0x30,0x24,0x06,0x03,0x55,0x04,0x03,0x0C,0x1D,0x73,0x65,0x6C,0x66,
	0x2D,0x73,0x69,0x67,0x6E,0x65,0x64,0x2E,0x73,0x73,0x6C,0x74,0x65,0x73,0x74,0x2E,
	0x61,0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,
	0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x1E,0x17,0x0D,0x31,0x36,0x30,0x36,0x30,0x37,
	0x32,0x31,0x35,0x33,0x30,0x38,0x5A,0x17,0x0D,0x31,0x37,0x30,0x36,0x30,0x37,0x32,
	0x31,0x35,0x33,0x30,0x38,0x5A,0x30,0x35,0x31,0x26,0x30,0x24,0x06,0x03,0x55,0x04,
	0x03,0x0C,0x1D,0x73,0x65,0x6C,0x66,0x2D,0x73,0x69,0x67,0x6E,0x65,0x64,0x2E,0x73,
	0x73,0x6C,0x74,0x65,0x73,0x74,0x2E,0x61,0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,
	0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x82,0x01,
	0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,
	0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xCC,0x72,
	0x7D,0x09,0x36,0x5A,0x6A,0xED,0xC1,0x7A,0x2C,0xF4,0x7C,0x58,0x63,0x05,0x3E,0x91,
	0x68,0x55,0xB1,0x2A,0x5D,0x57,0xF3,0xA4,0xA7,0x80,0x05,0x41,0x74,0xB2,0xAD,0x5A,
	0x7F,0x38,0xF6,0xF7,0xFD,0xF9,0x64,0x4D,0xDE,0xF9,0x7A,0xD3,0x8C,0x78,0xE9,0x71,
	0xCF,0x1D,0x3E,0xF0,0xDB,0x12,0x48,0x74,0x22,0xA8,0x1F,0x3F,0xB9,0xDD,0xB0,0xAD,
	0x8C,0x10,0x64,0x05,0x0E,0xE2,0x59,0x9A,0xEB,0x3F,0xBF,0xA9,0x48,0x07,0xD9,0x2C,
	0x07,0x44,0x70,0x14,0x16,0x56,0x9C,0x73,0x01,0x2E,0x0B,0xF1,0x2A,0x9F,0x1C,0xC6,
	0x78,0x56,0xB7,0x0B,0xDA,0xA6,0xE6,0x99,0x87,0x2D,0x49,0xFB,0xF0,0x47,0x22,0xA6,
	0x8B,0xF0,0x02,0x37,0x31,0xD0,0x34,0x9F,0x43,0xD1,0x24,0x49,0x94,0x7F,0xFD,0x48,
	0x9C,0xBA,0x5D,0x6B,0xD4,0xF9,0x9E,0xB5,0x18,0xE4,0xB2,0x06,0x46,0xC3,0xD9,0xE7,
	0x80,0xD8,0x61,0xA9,0x09,0x5E,0xBA,0x2E,0x58,0x56,0xAE,0x37,0x31,0x6E,0x87,0x98,
	0xD5,0xC9,0x2B,0x31,0x5C,0x40,0x01,0xDF,0xD5,0x63,0x9E,0x05,0x18,0x21,0x53,0x70,
	0x62,0x36,0x44,0xCD,0x02,0xC0,0xCC,0x6A,0x58,0xC6,0xF6,0xA4,0xDC,0x89,0x94,0xBD,
	0x4E,0xC4,0xEE,0xEE,0x40,0x31,0x59,0xC3,0x43,0xAD,0x34,0x30,0xDE,0xA9,0xA7,0x0D,
	0x85,0xF7,0x96,0x8C,0x45,0xC1,0x6E,0x85,0x39,0x97,0xA6,0x4F,0xEA,0xE8,0x2F,0x01,
	0x3D,0xC0,0x3B,0x34,0x9F,0x8F,0xCB,0xD6,0x22,0x79,0x2C,0x8C,0x8C,0xE6,0xBB,0x1F,
	0x89,0x87,0x93,0x3B,0x39,0x4E,0x64,0x7D,0xDA,0x4D,0x52,0x4C,0x97,0xE5,0x02,0x03,
	0x01,0x00,0x01,0xA3,0x2A,0x30,0x28,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,
	0xFF,0x04,0x04,0x03,0x02,0x07,0x80,0x30,0x16,0x06,0x03,0x55,0x1D,0x25,0x01,0x01,
	0xFF,0x04,0x0C,0x30,0x0A,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x03,0x01,0x30,
	0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,
	0x01,0x01,0x00,0x36,0x06,0xC9,0xE6,0x98,0xC2,0x84,0x1D,0x13,0x1E,0x54,0x35,0x6D,
	0xE5,0xCB,0xC5,0xFD,0xD9,0x54,0x45,0x83,0x53,0xB3,0x3B,0xE7,0x30,0x6F,0xAE,0xEA,
	0x63,0x3F,0xA8,0xFA,0xD9,0x6D,0x0F,0x7D,0xD4,0xB6,0x28,0x66,0xF9,0x57,0x87,0x3E,
	0x57,0x27,0xB6,0x9A,0x56,0xAE,0xD7,0xE0,0x11,0x20,0x71,0xC1,0xEA,0xF6,0xED,0x74,
	0x1A,0x5A,0xB1,0x74,0x6C,0xBE,0xAC,0x0E,0x3C,0xD9,0x3E,0xEC,0x17,0x6E,0xF0,0x69,
	0xC9,0x4D,0xD2,0x7E,0xAE,0x8B,0x01,0xCC,0x1A,0x23,0x7C,0x58,0x07,0x30,0xE4,0x2A,
	0x12,0xE8,0xA0,0x25,0x65,0x66,0xB5,0xC7,0x5D,0xD8,0x47,0xDF,0xD7,0x51,0xBC,0xA2,
	0xAA,0xF0,0x2F,0xB5,0x9E,0x20,0x6D,0x1F,0x84,0x00,0xF0,0xD0,0xB8,0x42,0x6A,0x9A,
	0xE7,0xCA,0x7B,0xE5,0x39,0x09,0x91,0xBF,0xCB,0x4D,0x7A,0x32,0x1E,0x00,0x6E,0xE5,
	0xF7,0x44,0x80,0x82,0x38,0x53,0x64,0xB7,0x26,0x81,0xCB,0xCE,0xA1,0xAF,0x0C,0x67,
	0x32,0xC6,0xE4,0x5D,0x09,0x7B,0x37,0xD7,0xC8,0x43,0x44,0xEF,0xC6,0xF8,0x72,0xFF,
	0x65,0xD4,0x39,0x3D,0xEC,0x72,0xA5,0x28,0xFF,0x70,0x47,0x38,0xA3,0xC7,0xCC,0x5E,
	0x0F,0xFF,0x43,0x83,0x78,0x49,0x68,0x90,0x48,0x89,0xAD,0xE1,0x2E,0xFA,0x8F,0x59,
	0xB6,0x08,0x2A,0x72,0x2F,0x52,0x3F,0x73,0x84,0xCA,0xD8,0x18,0x6C,0xDA,0xA3,0x2E,
	0xF2,0xD7,0x4C,0x21,0xD9,0xF8,0xB1,0x86,0xE9,0x35,0x78,0xE4,0x4F,0xD0,0x93,0x11,
	0x8F,0xF4,0xB1,0x17,0x4F,0xDE,0xAC,0xBD,0xA9,0xBC,0x94,0xFC,0x2E,0x7D,0xF9,0x05,
	0x26,0x90,0xF1,
};

#define CFReleaseSafe(CF) { CFTypeRef _cf = (CF); if (_cf) CFRelease(_cf); }
#define CFReleaseNull(CF) { CFTypeRef _cf = (CF); if (_cf) { (CF) = NULL; CFRelease(_cf); } }

/* Test basic add delete update copy matching stuff. */
static void tests(void)
{
	SecTrustRef trust;
	SecCertificateRef cert0, cert1, sscert0;
	CFArrayRef anchors = NULL;
	isnt(cert0 = SecCertificateCreateWithBytes(NULL, _c0, sizeof(_c0)),
			NULL, "create cert0");
	isnt(cert1 = SecCertificateCreateWithBytes(NULL, _c1, sizeof(_c1)),
			NULL, "create cert1");
	isnt(sscert0 = SecCertificateCreateWithBytes(NULL, _ss0, sizeof(_ss0)),
			NULL, "create sscert0");
	const void *v_certs[] = {
		cert0,
		cert1
	};
	SecPolicyRef policy = SecPolicyCreateSSL(true, CFSTR("store.apple.com"));
	CFArrayRef certs = CFArrayCreate(NULL, v_certs,
			array_size(v_certs), NULL);
	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust), "create trust");
	CFDateRef date = CFDateCreate(NULL, 545000000.0); /* April 9, 2018 at 1:53:20 PM PDT */
	ok_status(SecTrustSetVerifyDate(trust, date), "set date");

	SecTrustResultType trustResult;
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultUnspecified,
			"trust is kSecTrustResultUnspecified");
	CFDataRef exceptions;
	ok(exceptions = SecTrustCopyExceptions(trust), "create exceptions");
	if (!exceptions) { goto errOut; }
	ok(SecTrustSetExceptions(trust, exceptions), "set exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultProceed, "trust is kSecTrustResultProceed");

	CFReleaseNull(trust);
	CFReleaseNull(policy);
	policy = SecPolicyCreateSSL(true, CFSTR("badstore.apple.com"));
	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust), "create trust with hostname mismatch");
	ok_status(SecTrustSetVerifyDate(trust, date), "set date");
	ok(SecTrustSetExceptions(trust, exceptions), "set old exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultRecoverableTrustFailure, "trust is kSecTrustResultRecoverableTrustFailure");
	CFReleaseNull(exceptions);
	ok(exceptions = SecTrustCopyExceptions(trust), "create exceptions");
	if (!exceptions) { goto errOut; }
	ok(SecTrustSetExceptions(trust, exceptions), "set exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultProceed, "trust is kSecTrustResultProceed");

	CFReleaseNull(trust);
	ok_status(SecTrustCreateWithCertificates(certs, policy, &trust), "create trust");
	ok_status(SecTrustSetVerifyDate(trust, date), "set date");
	ok(SecTrustSetExceptions(trust, exceptions), "set exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultProceed, "trust is kSecTrustResultProceed");
	anchors = CFArrayCreate(kCFAllocatorDefault, NULL, 0, &kCFTypeArrayCallBacks);
	ok_status(SecTrustSetAnchorCertificates(trust, anchors), "set empty anchor list");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultRecoverableTrustFailure, "trust is kSecTrustResultRecoverableTrustFailure");

	ok_status(SecTrustSetAnchorCertificatesOnly(trust, false), "trust passed in anchors and system anchors");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultProceed, "trust is now kSecTrustResultProceed");

	ok_status(SecTrustSetAnchorCertificatesOnly(trust, true), "only trust passed in anchors (default)");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultRecoverableTrustFailure, "trust is kSecTrustResultRecoverableTrustFailure again");

	CFReleaseNull(exceptions);
	ok(exceptions = SecTrustCopyExceptions(trust), "create exceptions");
	if (!exceptions) { goto errOut; }
	ok(SecTrustSetExceptions(trust, exceptions), "set exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultProceed, "trust is kSecTrustResultProceed");
	CFReleaseNull(date);
	date = CFDateCreate(NULL, 667680000.0);
	ok_status(SecTrustSetVerifyDate(trust, date), "set date to far future so certs are expired");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultRecoverableTrustFailure, "trust is kSecTrustResultRecoverableTrustFailure");

	CFReleaseNull(trust);
	CFReleaseNull(policy);
	policy = SecPolicyCreateSSL(true, CFSTR("self-signed.ssltest.apple.com"));
	ok_status(SecTrustCreateWithCertificates(sscert0, policy, &trust), "create trust");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultRecoverableTrustFailure, "trust is kSecTrustResultRecoverableTrustFailure");
	CFReleaseNull(exceptions);
	ok(exceptions = SecTrustCopyExceptions(trust), "create exceptions");
	if (!exceptions) { goto errOut; }
	ok(SecTrustSetExceptions(trust, exceptions), "set exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultProceed, "trust is kSecTrustResultProceed");
    CFReleaseNull(exceptions);
	ok(!SecTrustSetExceptions(trust, NULL), "clear exceptions");
	ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
	is_status(trustResult, kSecTrustResultRecoverableTrustFailure, "trust is kSecTrustResultRecoverableTrustFailure");

errOut:
	CFReleaseSafe(anchors);
	CFReleaseSafe(exceptions);
	CFReleaseSafe(trust);
	CFReleaseSafe(policy);
	CFReleaseSafe(certs);
	CFReleaseSafe(cert0);
	CFReleaseSafe(cert1);
	CFReleaseSafe(sscert0);
	CFReleaseSafe(date);
}

int si_27_sectrust_exceptions(int argc, char *const *argv)
{
	plan_tests(51);


	tests();

	return 0;
}
