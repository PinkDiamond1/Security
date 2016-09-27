/*
 *  si-81-sectrust-server-auth.c
 *  Security
 *
 *  Copyright (c) 2014 Apple Inc. All Rights Reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <Security/SecCertificate.h>
#include <Security/SecCertificatePriv.h>
#include <Security/SecCertificateInternal.h>
#include <Security/SecItem.h>
#include <Security/SecItemPriv.h>
#include <Security/SecIdentityPriv.h>
#include <Security/SecIdentity.h>
#include <Security/SecPolicy.h>
#include <Security/SecPolicyPriv.h>
#include <Security/SecPolicyInternal.h>
#include <Security/SecCMS.h>
#include <utilities/SecCFWrappers.h>
#include <stdlib.h>
#include <unistd.h>

#include "Security_regressions.h"


static const UInt8 kTestAppleVPNLeafCert[] = {
    0x30,0x82,0x04,0x55,0x30,0x82,0x03,0x3D,0xA0,0x03,0x02,0x01,0x02,0x02,0x08,0x1F,
    0xF4,0x7D,0xBF,0x19,0x70,0x2F,0xE4,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x6E,0x31,0x28,0x30,0x26,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x1F,0x41,0x70,0x70,0x6C,0x65,0x20,0x43,0x6F,0x72,0x70,0x6F,0x72,0x61,
    0x74,0x65,0x20,0x56,0x50,0x4E,0x20,0x43,0x6C,0x69,0x65,0x6E,0x74,0x20,0x43,0x41,
    0x20,0x31,0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,0x0B,0x0C,0x17,0x43,0x65,0x72,
    0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,
    0x72,0x69,0x74,0x79,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x41,
    0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,
    0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x1E,0x17,0x0D,0x31,0x34,0x30,0x37,0x30,0x37,
    0x32,0x30,0x33,0x38,0x35,0x33,0x5A,0x17,0x0D,0x31,0x34,0x31,0x30,0x30,0x35,0x32,
    0x30,0x33,0x38,0x35,0x33,0x5A,0x30,0x61,0x31,0x5F,0x30,0x5D,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x56,0x63,0x6F,0x6D,0x2E,0x61,0x70,0x70,0x6C,0x65,0x2E,0x69,0x73,0x74,
    0x2E,0x64,0x73,0x2E,0x61,0x70,0x70,0x6C,0x65,0x63,0x6F,0x6E,0x6E,0x65,0x63,0x74,
    0x32,0x2E,0x70,0x72,0x6F,0x64,0x75,0x63,0x74,0x69,0x6F,0x6E,0x2E,0x76,0x70,0x6E,
    0x2E,0x38,0x46,0x32,0x42,0x33,0x41,0x44,0x43,0x44,0x37,0x32,0x45,0x44,0x32,0x45,
    0x41,0x30,0x38,0x44,0x44,0x43,0x32,0x36,0x41,0x44,0x30,0x32,0x35,0x35,0x41,0x39,
    0x38,0x33,0x42,0x31,0x44,0x45,0x42,0x45,0x42,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,
    0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,
    0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xE1,0xA1,0xCB,0x69,0x00,0x88,
    0x06,0x3A,0x7C,0xE3,0x29,0x1F,0x1C,0x32,0x22,0xD2,0x71,0xFA,0x06,0xED,0xC0,0x16,
    0x41,0x80,0x4C,0x82,0xA5,0x98,0x98,0x02,0xCE,0xB3,0x7D,0xC9,0x89,0x8D,0x91,0xDE,
    0x61,0x34,0xCB,0xB9,0x24,0xA8,0xA4,0x4F,0x7F,0x07,0x09,0x80,0x6A,0xAB,0x0C,0x3C,
    0xBC,0xD5,0xDA,0xAB,0xD1,0x69,0x47,0x7C,0x1F,0x93,0x4E,0xF0,0x44,0x50,0x54,0x91,
    0x85,0xB6,0x67,0x53,0x35,0x45,0x60,0x92,0xD8,0xDA,0x20,0xBC,0x7E,0x3E,0xA5,0xA0,
    0xD7,0x2B,0x62,0x78,0x94,0xA4,0x97,0x74,0x07,0xAA,0x7A,0x91,0xFC,0xF4,0xFA,0x50,
    0x8E,0x24,0x6D,0x2E,0x64,0x01,0x23,0x6D,0x16,0xA2,0x5A,0x18,0xFB,0x82,0xAB,0x7E,
    0x19,0xE9,0x7F,0x1E,0xEE,0x4C,0x0A,0xEA,0xD5,0xE4,0xE7,0x29,0xE5,0xA1,0x52,0xAD,
    0xED,0xD9,0xE9,0x56,0xA1,0xFC,0xD8,0x9D,0x89,0xC9,0x31,0x3F,0x98,0xE6,0xBD,0xAF,
    0x5B,0x6E,0xCC,0x43,0x0B,0xD9,0x55,0xC7,0x8A,0x1A,0x36,0x1A,0xE7,0x1B,0x46,0xD5,
    0xF8,0x90,0xD9,0x68,0x2E,0x29,0xBE,0xED,0xE0,0x15,0x2A,0x5F,0x8F,0xBA,0x4C,0x32,
    0x99,0xF5,0x83,0x85,0x74,0x8E,0x4A,0x69,0xC6,0x0E,0x3F,0x35,0x1C,0xD8,0x8F,0xB6,
    0x28,0x94,0x68,0xF5,0x65,0x33,0x12,0xE6,0x8D,0x9D,0x8F,0x99,0x71,0xFC,0xAA,0x2B,
    0xF8,0x79,0xFD,0x89,0x66,0x0E,0xDC,0x17,0xF4,0x73,0xFD,0x11,0x2A,0x8A,0xC5,0x90,
    0x8C,0xC3,0x33,0x48,0x02,0x19,0x2D,0x86,0xD7,0xED,0xD8,0x4D,0xF4,0x21,0xB0,0x1D,
    0xB8,0x3B,0xCC,0x7A,0xBA,0x10,0x9C,0x32,0x95,0x7D,0x02,0x03,0x01,0x00,0x01,0xA3,
    0x82,0x01,0x02,0x30,0x81,0xFF,0x30,0x4B,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,
    0x01,0x01,0x04,0x3F,0x30,0x3D,0x30,0x3B,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,
    0x30,0x01,0x86,0x2F,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x6F,0x63,0x73,0x70,0x2E,
    0x61,0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x2F,0x6F,0x63,0x73,0x70,0x30,0x31,
    0x2D,0x63,0x6F,0x72,0x70,0x76,0x70,0x6E,0x63,0x6C,0x69,0x65,0x6E,0x74,0x63,0x61,
    0x31,0x30,0x31,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x9D,0x04,
    0x52,0xAB,0xBA,0x68,0x49,0x05,0x7E,0x09,0xE8,0x74,0x00,0x30,0x87,0xAF,0x16,0xF0,
    0xD5,0x9D,0x30,0x0C,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x02,0x30,0x00,
    0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0xA9,0x85,0xE3,
    0x5B,0xDB,0xFF,0xFC,0x2D,0x7F,0x80,0x04,0xAA,0xF0,0xDA,0xAD,0x37,0x78,0x82,0xA3,
    0xE0,0x30,0x3A,0x06,0x03,0x55,0x1D,0x1F,0x04,0x33,0x30,0x31,0x30,0x2F,0xA0,0x2D,
    0xA0,0x2B,0x86,0x29,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x63,0x72,0x6C,0x2E,0x61,
    0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x2F,0x63,0x6F,0x72,0x70,0x76,0x70,0x6E,
    0x63,0x6C,0x69,0x65,0x6E,0x74,0x63,0x61,0x31,0x2E,0x63,0x72,0x6C,0x30,0x0E,0x06,
    0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x07,0x80,0x30,0x16,0x06,
    0x03,0x55,0x1D,0x25,0x01,0x01,0xFF,0x04,0x0C,0x30,0x0A,0x06,0x08,0x2B,0x06,0x01,
    0x05,0x05,0x07,0x03,0x02,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,
    0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x11,0x76,0x58,0xDA,0x13,0xD2,0x3F,
    0x8E,0x4F,0xA0,0xAE,0x38,0xE5,0x74,0x85,0x94,0x60,0x3F,0xD7,0x30,0xBD,0x56,0x65,
    0xCB,0x28,0xD9,0x5B,0xCF,0x7E,0xEB,0x49,0x03,0x6A,0x6E,0xFB,0x17,0xC7,0xEA,0x3F,
    0x04,0xCF,0xC1,0xA2,0xFC,0x7C,0x8F,0xC0,0x19,0x79,0xF1,0xD5,0x07,0xF5,0x83,0xA4,
    0x33,0x6A,0xBC,0x0F,0x24,0x8E,0xA0,0x63,0xD3,0x00,0xA0,0xD8,0x08,0x92,0x21,0xB6,
    0x5B,0x56,0xE2,0x79,0x58,0xF2,0x3F,0x0F,0x98,0xC1,0x69,0xB6,0x10,0xA3,0x8C,0xA4,
    0xE4,0xEE,0x50,0xF6,0x52,0x5D,0x84,0x48,0x69,0x59,0x6D,0x21,0x7F,0xFB,0x1B,0xA3,
    0x21,0xBA,0xAE,0xCC,0x0A,0xD2,0xB4,0xBF,0xAA,0x7D,0x63,0xEE,0x74,0x9C,0x62,0x21,
    0xCB,0x93,0x14,0x7F,0x8E,0x38,0xD9,0x1D,0xF0,0x77,0xB1,0x77,0x92,0xE8,0xFE,0xFE,
    0xAE,0x6D,0xAF,0x5A,0x6F,0xE8,0xBC,0x4E,0xAC,0xAF,0xDF,0xF1,0xE1,0x4C,0x2A,0x26,
    0x9A,0xA1,0xD7,0x35,0xFD,0xE7,0x2B,0xBD,0x40,0xBB,0xE1,0x2A,0xEA,0xB0,0xEF,0xEF,
    0xE8,0x40,0x29,0xB7,0xDC,0xA2,0xC5,0x68,0x01,0xEB,0xBE,0x19,0x3F,0x22,0xFF,0x2D,
    0x43,0x2D,0xBA,0xB3,0x33,0xCF,0xDD,0xD7,0xBC,0x4B,0xEC,0x44,0x47,0x74,0x78,0x39,
    0x59,0xE3,0xC7,0x5A,0x9A,0x65,0xCD,0xFF,0xE5,0x80,0x63,0xEF,0xCB,0x8A,0xA4,0xF6,
    0x69,0xFF,0x65,0x26,0xA5,0xDA,0x53,0xBE,0x03,0xF6,0x81,0x52,0x2A,0x2C,0x71,0x59,
    0x7B,0x2A,0x4A,0xE1,0x9A,0x2A,0x52,0xEE,0x7A,0x48,0x80,0x86,0xD0,0x9D,0x66,0xB5,
    0xD3,0xE7,0xAC,0x03,0x14,0x04,0x4F,0xC6,0x65,
};

static const UInt8 kTestAppleVPNCACert[] = {
    0x30,0x82,0x04,0x44,0x30,0x82,0x03,0x2C,0xA0,0x03,0x02,0x01,0x02,0x02,0x08,0x0E,
    0x74,0x07,0xCA,0x6E,0x0B,0xA3,0x20,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x66,0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x17,0x41,0x70,0x70,0x6C,0x65,0x20,0x43,0x6F,0x72,0x70,0x6F,0x72,0x61,
    0x74,0x65,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x31,0x20,0x30,0x1E,0x06,0x03,
    0x55,0x04,0x0B,0x0C,0x17,0x43,0x65,0x72,0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,
    0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x31,0x13,0x30,0x11,
    0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x41,0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,
    0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x1E,
    0x17,0x0D,0x31,0x34,0x30,0x33,0x31,0x39,0x30,0x30,0x30,0x30,0x35,0x30,0x5A,0x17,
    0x0D,0x32,0x39,0x30,0x33,0x31,0x39,0x30,0x30,0x30,0x30,0x35,0x30,0x5A,0x30,0x6E,
    0x31,0x28,0x30,0x26,0x06,0x03,0x55,0x04,0x03,0x0C,0x1F,0x41,0x70,0x70,0x6C,0x65,
    0x20,0x43,0x6F,0x72,0x70,0x6F,0x72,0x61,0x74,0x65,0x20,0x56,0x50,0x4E,0x20,0x43,
    0x6C,0x69,0x65,0x6E,0x74,0x20,0x43,0x41,0x20,0x31,0x31,0x20,0x30,0x1E,0x06,0x03,
    0x55,0x04,0x0B,0x0C,0x17,0x43,0x65,0x72,0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,
    0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x31,0x13,0x30,0x11,
    0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x41,0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,
    0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x82,
    0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,
    0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xA4,
    0xBF,0xFE,0x2F,0x4D,0x46,0x90,0x89,0x1C,0x89,0xDF,0x4E,0x70,0x02,0x73,0x9F,0x2A,
    0x10,0xFA,0x72,0x60,0xEE,0x99,0x16,0xE8,0x05,0xB7,0x7D,0x73,0x96,0x03,0x6A,0x46,
    0x7F,0x2E,0xAE,0xC9,0x42,0xF1,0x05,0xAA,0x64,0xED,0x4B,0xF8,0xE7,0xAF,0x2D,0x16,
    0xB2,0xC2,0x4F,0x52,0x31,0xEE,0x9E,0xCC,0x23,0x7E,0x40,0xA2,0x7B,0xCE,0x89,0xD8,
    0x0F,0xE4,0xB8,0x27,0x50,0x33,0xB3,0x24,0x01,0x61,0xB1,0xD5,0x81,0x7A,0x75,0x1F,
    0xFF,0x3C,0x99,0x79,0x9D,0x7A,0x17,0x44,0x1A,0xC0,0xD2,0xF4,0x64,0x69,0x2C,0x9C,
    0x75,0x22,0xC8,0x51,0x0D,0xD7,0xB6,0x8D,0x9B,0xF6,0x13,0x9F,0xA8,0xC1,0x0F,0xC0,
    0x4A,0x5A,0x83,0x98,0x01,0x4C,0x04,0xDF,0xCD,0x74,0xC5,0x4C,0x3D,0x72,0x05,0x41,
    0xCA,0x64,0x17,0xED,0x63,0x1C,0x8A,0xCE,0x93,0x05,0x18,0xF7,0xD8,0x35,0x75,0xD0,
    0x7D,0x03,0x57,0xE7,0x15,0x20,0xC1,0x1A,0x02,0xAD,0x94,0x91,0x06,0x88,0xAF,0xCB,
    0x41,0x24,0xF1,0xBB,0x17,0xC3,0x73,0x4A,0x63,0x08,0xE0,0xAF,0x14,0xCA,0x03,0x8C,
    0xC9,0xA5,0xDC,0x5C,0xE3,0x7F,0xF6,0x33,0x99,0x07,0xD2,0xA5,0x15,0xEB,0x70,0xB7,
    0x3E,0x5D,0xD5,0x50,0x81,0x9C,0xE3,0x7C,0x3B,0x21,0x98,0xB3,0xE2,0xF3,0x00,0x8D,
    0xBE,0x68,0xDD,0x9B,0xFC,0x87,0xBF,0xF0,0x01,0x58,0x53,0xAC,0x2B,0x01,0xE9,0x81,
    0xB4,0x49,0x4A,0x02,0x98,0xC3,0x5B,0x92,0xFD,0x0B,0x9C,0xDC,0x56,0x96,0x5B,0x3B,
    0x02,0x81,0x1F,0x00,0xC2,0x2C,0xF0,0x1C,0xB9,0x38,0x20,0xE7,0x51,0x90,0x91,0x02,
    0x03,0x01,0x00,0x01,0xA3,0x81,0xED,0x30,0x81,0xEA,0x30,0x41,0x06,0x08,0x2B,0x06,
    0x01,0x05,0x05,0x07,0x01,0x01,0x04,0x35,0x30,0x33,0x30,0x31,0x06,0x08,0x2B,0x06,
    0x01,0x05,0x05,0x07,0x30,0x01,0x86,0x25,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x6F,
    0x63,0x73,0x70,0x2E,0x61,0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x2F,0x6F,0x63,
    0x73,0x70,0x30,0x34,0x2D,0x63,0x6F,0x72,0x70,0x72,0x6F,0x6F,0x74,0x30,0x1D,0x06,
    0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0xA9,0x85,0xE3,0x5B,0xDB,0xFF,0xFC,0x2D,
    0x7F,0x80,0x04,0xAA,0xF0,0xDA,0xAD,0x37,0x78,0x82,0xA3,0xE0,0x30,0x0F,0x06,0x03,
    0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x05,0x30,0x03,0x01,0x01,0xFF,0x30,0x1F,0x06,
    0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x35,0x20,0x26,0xCE,0x85,0xBE,
    0x49,0x26,0x20,0x01,0xDD,0xC8,0xEE,0xFF,0x3D,0x68,0xC8,0xD0,0xDF,0xF5,0x30,0x32,
    0x06,0x03,0x55,0x1D,0x1F,0x04,0x2B,0x30,0x29,0x30,0x27,0xA0,0x25,0xA0,0x23,0x86,
    0x21,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x63,0x72,0x6C,0x2E,0x61,0x70,0x70,0x6C,
    0x65,0x2E,0x63,0x6F,0x6D,0x2F,0x63,0x6F,0x72,0x70,0x72,0x6F,0x6F,0x74,0x2E,0x63,
    0x72,0x6C,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,
    0x01,0x06,0x30,0x10,0x06,0x0A,0x2A,0x86,0x48,0x86,0xF7,0x63,0x64,0x06,0x18,0x05,
    0x04,0x02,0x05,0x00,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,
    0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x38,0x50,0xD1,0x41,0x40,0xAD,0x98,0x12,
    0xB6,0xB5,0x7A,0xC6,0x42,0xD7,0x40,0x25,0x2A,0xDB,0x7C,0xEA,0x84,0xA8,0x21,0xCE,
    0xA9,0xBE,0x26,0x0D,0x10,0xFD,0xFE,0x9C,0x8D,0x8A,0xA5,0xA6,0x3C,0x44,0xDA,0xF1,
    0xD1,0x05,0xEF,0xB3,0x17,0x8F,0x21,0x88,0x3B,0xF8,0x48,0x5B,0xE3,0xF6,0xC1,0x1B,
    0x6C,0xE4,0xD0,0xF7,0xD2,0xA1,0xF9,0x89,0xFA,0x71,0x12,0x4F,0x90,0x95,0x73,0x89,
    0xE9,0xD8,0xB5,0xBD,0x4B,0xF6,0x39,0x9D,0x97,0xFB,0x1C,0x5C,0x46,0x48,0x83,0x13,
    0x02,0xBF,0xA5,0xB1,0x74,0xD9,0x8A,0x41,0xA4,0x80,0x1E,0xD4,0xA6,0xC5,0x2C,0xD3,
    0x42,0xEF,0x17,0x8B,0xB0,0xFB,0x49,0x12,0x60,0x29,0x35,0xA6,0xAF,0x23,0x04,0xF1,
    0x26,0x8A,0x84,0x81,0x54,0x16,0x62,0x7B,0xBD,0xCE,0x09,0x39,0x3C,0x74,0x05,0xF4,
    0x75,0xE8,0x8A,0x77,0x13,0x07,0x93,0xB8,0x95,0x8E,0xD5,0x29,0x3E,0x17,0x6D,0x1B,
    0xA0,0xCB,0x4A,0xBB,0x33,0x28,0x06,0xCA,0x76,0x20,0x6F,0x07,0x32,0x04,0xE3,0xD9,
    0xD2,0x89,0xAE,0xD7,0x17,0x16,0x4A,0x4E,0x80,0xD6,0x9F,0x1F,0x75,0x6E,0x7C,0xCF,
    0x17,0xCE,0xD7,0x5E,0x5E,0x7B,0x55,0xC2,0x0D,0xCA,0xE2,0xEB,0x87,0x60,0xAC,0x32,
    0x31,0x1E,0x9F,0xF0,0x77,0xA3,0x28,0x6A,0x34,0xD0,0xF9,0xEE,0x47,0x6D,0xE6,0x1E,
    0x8A,0x87,0x82,0x49,0x72,0xBA,0x09,0x3B,0xB9,0x3B,0xF9,0xA4,0xEC,0x9B,0x5F,0x6B,
    0xEE,0xC1,0x6E,0xAC,0xC5,0x45,0xF4,0x44,0x8B,0x9B,0x27,0xBB,0xA4,0xBE,0x8D,0x74,
    0xE1,0x5D,0x4A,0xA2,0x35,0x4D,0xC8,0x42,
};

static const UInt8 kTestAppleCorpCACert[] = {
    0x30,0x82,0x03,0xB1,0x30,0x82,0x02,0x99,0xA0,0x03,0x02,0x01,0x02,0x02,0x08,0x14,
    0x99,0x6B,0x4A,0x6A,0xE4,0x40,0xA0,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x66,0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x17,0x41,0x70,0x70,0x6C,0x65,0x20,0x43,0x6F,0x72,0x70,0x6F,0x72,0x61,
    0x74,0x65,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x31,0x20,0x30,0x1E,0x06,0x03,
    0x55,0x04,0x0B,0x0C,0x17,0x43,0x65,0x72,0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,
    0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x31,0x13,0x30,0x11,
    0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x41,0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,
    0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x1E,
    0x17,0x0D,0x31,0x33,0x30,0x37,0x31,0x36,0x31,0x39,0x32,0x30,0x34,0x35,0x5A,0x17,
    0x0D,0x32,0x39,0x30,0x37,0x31,0x37,0x31,0x39,0x32,0x30,0x34,0x35,0x5A,0x30,0x66,
    0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,0x03,0x0C,0x17,0x41,0x70,0x70,0x6C,0x65,
    0x20,0x43,0x6F,0x72,0x70,0x6F,0x72,0x61,0x74,0x65,0x20,0x52,0x6F,0x6F,0x74,0x20,
    0x43,0x41,0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,0x0B,0x0C,0x17,0x43,0x65,0x72,
    0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,
    0x72,0x69,0x74,0x79,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x41,
    0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,
    0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,
    0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,
    0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xB5,0x3B,0xE3,0x9F,0x6A,0x1D,0x0E,0x46,0x51,
    0x1E,0xD0,0xB5,0x17,0x6B,0x06,0x4B,0x92,0xAF,0x38,0x10,0x25,0xA1,0xEE,0x1E,0x4E,
    0xEF,0x19,0xE0,0x73,0xB5,0x37,0x33,0x72,0x21,0x21,0xCB,0x62,0x4A,0x3D,0xA9,0x68,
    0xD8,0x07,0xB4,0xEB,0x8D,0x0A,0xDB,0x30,0x33,0x21,0x2F,0x6F,0xD3,0xF7,0x5D,0xCE,
    0x20,0x0A,0x04,0xDB,0xFF,0xBF,0x75,0x08,0x42,0x3F,0x3E,0xD8,0xC8,0xEF,0xA4,0xF8,
    0x56,0x7B,0x13,0x64,0x6B,0xF3,0xA2,0x38,0x10,0xFA,0xEE,0x9D,0x83,0x93,0x1D,0xFB,
    0xEF,0x13,0x6C,0x38,0x49,0xDD,0xEB,0x71,0xA6,0x92,0x58,0x04,0xDE,0x01,0x41,0x2B,
    0x99,0x5E,0xBD,0x24,0x3F,0x69,0xA8,0x44,0xF2,0xAA,0x01,0x78,0xB9,0x38,0x06,0x10,
    0x77,0x36,0xF8,0xF2,0xA3,0x3E,0xD9,0x5F,0xEA,0xF5,0x8B,0x6A,0xA6,0x5F,0xE6,0x51,
    0xD0,0x9B,0x50,0xA0,0x1E,0xF5,0x85,0x9E,0x49,0x50,0x4A,0x61,0x78,0xDA,0x29,0xA7,
    0x33,0x72,0x8B,0x83,0xEE,0x7B,0xA7,0x79,0x4E,0x8E,0x02,0x6F,0x9D,0x25,0x97,0x26,
    0x86,0x0C,0x82,0xC5,0x8C,0x16,0x7E,0x49,0x61,0xFD,0xFF,0x1A,0xA0,0x0D,0x28,0xE1,
    0x68,0xF5,0xAE,0x85,0x72,0xF3,0xAB,0xE0,0x74,0x75,0xCC,0x57,0x64,0x3C,0x2C,0x55,
    0x05,0xC9,0x8D,0xAA,0xB3,0xEC,0xC8,0x62,0x88,0x15,0x2A,0xC4,0x59,0x60,0x37,0xC1,
    0xED,0x6B,0xCE,0xE9,0xCA,0xAF,0xB0,0xA5,0x45,0xBA,0xFF,0x16,0x32,0xAA,0x92,0x86,
    0xD9,0xB9,0xA1,0x13,0x75,0x95,0x9B,0x97,0x5C,0x2D,0xB5,0x12,0xCA,0x6B,0x6B,0x39,
    0xD6,0x9B,0x4B,0x34,0x47,0xAB,0x35,0x02,0x03,0x01,0x00,0x01,0xA3,0x63,0x30,0x61,
    0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x35,0x20,0x26,0xCE,0x85,
    0xBE,0x49,0x26,0x20,0x01,0xDD,0xC8,0xEE,0xFF,0x3D,0x68,0xC8,0xD0,0xDF,0xF5,0x30,
    0x0F,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x05,0x30,0x03,0x01,0x01,0xFF,
    0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x35,0x20,0x26,
    0xCE,0x85,0xBE,0x49,0x26,0x20,0x01,0xDD,0xC8,0xEE,0xFF,0x3D,0x68,0xC8,0xD0,0xDF,
    0xF5,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x01,
    0x06,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,
    0x03,0x82,0x01,0x01,0x00,0x73,0x02,0x4A,0xA6,0x77,0x02,0xA7,0xE1,0xCB,0x52,0x97,
    0x9D,0x89,0x11,0xA0,0x8F,0xBC,0xF3,0x8F,0x14,0x01,0x29,0xF3,0xA5,0x45,0x17,0x06,
    0xF8,0x04,0xF2,0x6D,0xD5,0xC3,0x77,0xB8,0x00,0xC2,0x0A,0x1A,0x09,0x32,0x36,0x36,
    0x69,0xC1,0x2A,0xF0,0x44,0x37,0xBC,0x7E,0x5F,0x15,0xF7,0x08,0x9C,0x19,0x27,0x1D,
    0x70,0x4F,0xDC,0x17,0x94,0x3C,0xBB,0x24,0xB4,0xE6,0xFC,0x71,0x9A,0xD4,0xCF,0x2C,
    0x12,0xBA,0xF0,0xB6,0x8F,0x78,0x99,0xAA,0x8C,0x17,0x7E,0x94,0x0C,0x6A,0x37,0x5B,
    0x35,0x91,0x52,0xFA,0x64,0xA3,0x33,0x34,0x99,0x37,0x00,0x3C,0xB4,0x4E,0x6E,0x63,
    0xED,0xC3,0x1D,0x37,0x5B,0x45,0xB4,0xDF,0x82,0xCD,0xFE,0xAA,0x92,0x64,0xC8,0x2F,
    0xD6,0x2D,0x2E,0xB1,0xED,0x6A,0x04,0xF1,0xC2,0x48,0x8D,0x4B,0xB4,0x84,0x39,0xA3,
    0x31,0x4D,0xF6,0x63,0xB4,0xC3,0x6E,0xA1,0xA5,0x2F,0xD2,0x1E,0xB0,0xC6,0x0C,0xD1,
    0x04,0x3A,0x31,0xBC,0x87,0x49,0xF8,0x26,0x0B,0xD3,0x0C,0x08,0x29,0xBB,0x9F,0x4D,
    0x08,0xF0,0x9C,0x11,0xD3,0xA5,0x2C,0x8D,0x98,0xB1,0x1B,0xB1,0x57,0xD3,0x69,0xAE,
    0x9E,0x2D,0xD5,0x64,0x38,0x58,0xC9,0xB2,0x84,0x04,0xAB,0x10,0x1D,0xCA,0x6B,0x29,
    0xA5,0xAB,0xCC,0xFE,0xBB,0x74,0xF4,0x35,0x03,0x8F,0x65,0x2A,0x0B,0xBB,0xC7,0x17,
    0x6A,0x49,0x34,0x83,0x30,0x92,0x8D,0xD7,0xAE,0x95,0xD0,0xD7,0x23,0xA7,0xE3,0x29,
    0x09,0xA1,0xB1,0x34,0xC3,0x95,0x49,0xC3,0xA4,0xF1,0x36,0x00,0x09,0xD3,0xA4,0x09,
    0xAD,0xF2,0x5C,0x97,0xB2,
};


static const UInt8 kTestAppleServerAuthCACert[] = {
    0x30,0x82,0x03,0xF8,0x30,0x82,0x02,0xE0,0xA0,0x03,0x02,0x01,0x02,0x02,0x08,0x23,
    0x69,0x74,0x04,0xAD,0xCB,0x83,0x14,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x62,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,
    0x06,0x13,0x02,0x55,0x53,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x13,0x0A,
    0x41,0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x26,0x30,0x24,0x06,0x03,
    0x55,0x04,0x0B,0x13,0x1D,0x41,0x70,0x70,0x6C,0x65,0x20,0x43,0x65,0x72,0x74,0x69,
    0x66,0x69,0x63,0x61,0x74,0x69,0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,0x69,
    0x74,0x79,0x31,0x16,0x30,0x14,0x06,0x03,0x55,0x04,0x03,0x13,0x0D,0x41,0x70,0x70,
    0x6C,0x65,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x30,0x1E,0x17,0x0D,0x31,0x34,
    0x30,0x33,0x30,0x38,0x30,0x31,0x35,0x33,0x30,0x34,0x5A,0x17,0x0D,0x32,0x39,0x30,
    0x33,0x30,0x38,0x30,0x31,0x35,0x33,0x30,0x34,0x5A,0x30,0x6D,0x31,0x27,0x30,0x25,
    0x06,0x03,0x55,0x04,0x03,0x0C,0x1E,0x41,0x70,0x70,0x6C,0x65,0x20,0x53,0x65,0x72,
    0x76,0x65,0x72,0x20,0x41,0x75,0x74,0x68,0x65,0x6E,0x74,0x69,0x63,0x61,0x74,0x69,
    0x6F,0x6E,0x20,0x43,0x41,0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,0x0B,0x0C,0x17,
    0x43,0x65,0x72,0x74,0x69,0x66,0x69,0x63,0x61,0x74,0x69,0x6F,0x6E,0x20,0x41,0x75,
    0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,
    0x0C,0x0A,0x41,0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x0B,0x30,0x09,
    0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,
    0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,
    0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xB9,0x26,0x16,0xB0,0xCB,0x87,
    0xAB,0x71,0x15,0x92,0x8E,0xDF,0xAA,0x3E,0xE1,0x80,0xD7,0x53,0xBA,0xA4,0x60,0xCC,
    0x7C,0x85,0x72,0xF7,0x30,0x7C,0x09,0x4F,0x57,0x0D,0x4A,0xFF,0xE1,0x5E,0xC9,0x4B,
    0x50,0x13,0x02,0x64,0xB1,0xBD,0x39,0x35,0xD1,0xD7,0x04,0x51,0xC1,0x18,0xFA,0x22,
    0xFA,0xAE,0xDF,0x98,0x18,0xD6,0xBF,0x4E,0x4D,0x43,0x10,0xFA,0x25,0x88,0x9F,0xD3,
    0x40,0x85,0x76,0xE5,0x22,0x81,0xB6,0x54,0x45,0x73,0x9A,0x8B,0xE3,0x9C,0x48,0x1A,
    0x86,0x7A,0xC3,0x51,0xE2,0xDA,0x95,0xF8,0xA4,0x7D,0xDB,0x30,0xDE,0x6C,0x0E,0xC4,
    0xC5,0xF5,0x6C,0x98,0xE7,0xA6,0xFA,0x57,0x20,0x1D,0x19,0x73,0x7A,0x0E,0xCD,0x63,
    0x0F,0xB7,0x27,0x88,0x2E,0xE1,0x9A,0x68,0x82,0xB8,0x40,0x6C,0x63,0x16,0x24,0x66,
    0x2B,0xE7,0xB2,0xE2,0x54,0x7D,0xE7,0x88,0x39,0xA2,0x1B,0x81,0x3E,0x02,0xD3,0x39,
    0xD8,0x97,0x77,0x4A,0x32,0x0C,0xD6,0x0A,0x0A,0xB3,0x04,0x9B,0xF1,0x72,0x6F,0x63,
    0xA8,0x15,0x1E,0x6C,0x37,0xE8,0x0F,0xDB,0x53,0x90,0xD6,0x29,0x5C,0xBC,0x6A,0x57,
    0x9B,0x46,0x78,0x0A,0x3E,0x24,0xEA,0x9A,0x3F,0xA1,0xD8,0x3F,0xF5,0xDB,0x6E,0xA8,
    0x6C,0x82,0xB5,0xDD,0x99,0x38,0xEC,0x92,0x56,0x94,0xA6,0xC5,0x73,0x26,0xD1,0xAE,
    0x08,0xB2,0xC6,0x52,0xE7,0x8E,0x76,0x4B,0x89,0xB8,0x54,0x0F,0x6E,0xE0,0xD9,0x42,
    0xDB,0x2A,0x65,0x87,0x46,0x14,0xBB,0x96,0xB8,0x57,0xBB,0x51,0xE6,0x84,0x13,0xF7,
    0x0D,0xA1,0xB6,0x89,0xAC,0x7C,0xD1,0x21,0x74,0xAB,0x02,0x03,0x01,0x00,0x01,0xA3,
    0x81,0xA6,0x30,0x81,0xA3,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,
    0x2C,0xC5,0x6D,0x52,0xDD,0x31,0xEF,0x8C,0xEC,0x08,0x81,0xED,0xDF,0xDC,0xCA,0x43,
    0x00,0x45,0x01,0xD0,0x30,0x0F,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x05,
    0x30,0x03,0x01,0x01,0xFF,0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,
    0x80,0x14,0x2B,0xD0,0x69,0x47,0x94,0x76,0x09,0xFE,0xF4,0x6B,0x8D,0x2E,0x40,0xA6,
    0xF7,0x47,0x4D,0x7F,0x08,0x5E,0x30,0x2E,0x06,0x03,0x55,0x1D,0x1F,0x04,0x27,0x30,
    0x25,0x30,0x23,0xA0,0x21,0xA0,0x1F,0x86,0x1D,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,
    0x63,0x72,0x6C,0x2E,0x61,0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x2F,0x72,0x6F,
    0x6F,0x74,0x2E,0x63,0x72,0x6C,0x30,0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,
    0x04,0x04,0x03,0x02,0x01,0x06,0x30,0x10,0x06,0x0A,0x2A,0x86,0x48,0x86,0xF7,0x63,
    0x64,0x06,0x02,0x0C,0x04,0x02,0x05,0x00,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,
    0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x23,0xF1,0x06,0x7E,
    0x50,0x41,0x81,0xA2,0x5E,0xD3,0x70,0xA4,0x49,0x91,0xAF,0xD8,0xCC,0x67,0x8C,0xA1,
    0x25,0x7D,0xC4,0x9A,0x93,0x39,0x2F,0xD8,0x69,0xFB,0x1B,0x41,0x5B,0x44,0xD7,0xD9,
    0x6B,0xCB,0x3B,0x25,0x09,0x1A,0xF2,0xF4,0xE3,0xC7,0x9C,0xE8,0xB0,0x5B,0xF0,0xDF,
    0xDD,0x22,0x25,0x11,0x15,0x93,0xB9,0x49,0x5E,0xDA,0x0C,0x66,0x7A,0x5E,0xD7,0x6F,
    0xF0,0x63,0xD4,0x65,0x8C,0xC4,0x7A,0x54,0x7D,0x56,0x4F,0x65,0x9A,0xFD,0xDA,0xC4,
    0xB2,0xC8,0xB0,0xB8,0xA1,0xCB,0x7D,0xE0,0x47,0xA8,0x40,0x15,0xB8,0x16,0x19,0xED,
    0x5B,0x61,0x8E,0xDF,0xAA,0xD0,0xCD,0xD2,0x3A,0xC0,0x7E,0x3A,0x9F,0x22,0x4E,0xDF,
    0xDF,0xF4,0x4E,0x1A,0xCD,0x93,0xFF,0xD0,0xF0,0x45,0x55,0x64,0x33,0x3E,0xD4,0xE5,
    0xDA,0x68,0xA0,0x13,0x8A,0x76,0x30,0x27,0xD4,0xBF,0xF8,0x1E,0x76,0xF6,0xF9,0xC3,
    0x00,0xEF,0xB1,0x83,0xEA,0x53,0x6D,0x5C,0x35,0xC7,0x0D,0x07,0x01,0xBA,0xF8,0x61,
    0xB9,0xFE,0xC5,0x9A,0x6B,0x43,0x61,0x81,0x03,0xEB,0xBA,0x5F,0x70,0x9D,0xE8,0x6F,
    0x94,0x24,0x4B,0xDC,0xCE,0x92,0xA8,0x2E,0xA2,0x35,0x3C,0xE3,0x49,0xE0,0x16,0x77,
    0xA2,0xDC,0x6B,0xB9,0x8D,0x18,0x42,0xB9,0x36,0x96,0x43,0x32,0xC6,0xCB,0x76,0x99,
    0x35,0x36,0xD8,0x56,0xC6,0x98,0x5D,0xC3,0x6F,0xA5,0x7E,0x95,0xC2,0xD5,0x7A,0x0A,
    0x02,0x20,0x66,0x78,0x92,0xF2,0x67,0xA4,0x23,0x0D,0xE8,0x09,0xBD,0xCC,0x21,0x31,
    0x10,0xA0,0xBD,0xBE,0xB5,0xDD,0x4C,0xDD,0x46,0x03,0x99,0x99,
};

static const UInt8 kTestAppleServerAuthLeafCert[] = {
    0x30,0x82,0x04,0x09,0x30,0x82,0x02,0xF1,0xA0,0x03,0x02,0x01,0x02,0x02,0x08,0x16,
    0x18,0xA0,0xD4,0x51,0xA2,0x9E,0x3B,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x6D,0x31,0x27,0x30,0x25,0x06,0x03,0x55,0x04,
    0x03,0x0C,0x1E,0x41,0x70,0x70,0x6C,0x65,0x20,0x53,0x65,0x72,0x76,0x65,0x72,0x20,
    0x41,0x75,0x74,0x68,0x65,0x6E,0x74,0x69,0x63,0x61,0x74,0x69,0x6F,0x6E,0x20,0x43,
    0x41,0x31,0x20,0x30,0x1E,0x06,0x03,0x55,0x04,0x0B,0x0C,0x17,0x43,0x65,0x72,0x74,
    0x69,0x66,0x69,0x63,0x61,0x74,0x69,0x6F,0x6E,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,
    0x69,0x74,0x79,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,0x41,0x70,
    0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,
    0x06,0x13,0x02,0x55,0x53,0x30,0x1E,0x17,0x0D,0x31,0x34,0x30,0x33,0x31,0x39,0x32,
    0x33,0x34,0x36,0x32,0x38,0x5A,0x17,0x0D,0x31,0x35,0x30,0x34,0x31,0x38,0x32,0x33,
    0x34,0x36,0x32,0x38,0x5A,0x30,0x50,0x31,0x1A,0x30,0x18,0x06,0x03,0x55,0x04,0x03,
    0x0C,0x11,0x74,0x65,0x73,0x74,0x2E,0x6E,0x6F,0x73,0x75,0x63,0x68,0x64,0x6F,0x6D,
    0x61,0x69,0x6E,0x31,0x10,0x30,0x0E,0x06,0x03,0x55,0x04,0x0B,0x0C,0x07,0x43,0x6F,
    0x72,0x65,0x20,0x4F,0x53,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x0C,0x0A,
    0x41,0x70,0x70,0x6C,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x0B,0x30,0x09,0x06,0x03,
    0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,
    0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,
    0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,0xB8,0x1C,0xEF,0xB4,0x53,0x87,0x41,0x75,
    0x73,0x4E,0xDA,0xB3,0xAB,0x82,0x30,0x19,0x37,0x9D,0x29,0x2E,0x95,0xD2,0x2A,0x43,
    0x9C,0x1B,0x39,0xD4,0xBA,0xCD,0xAD,0x82,0xE4,0xBC,0x6F,0xA1,0x94,0x1C,0x75,0xBB,
    0xCD,0x21,0x95,0x1C,0x7B,0x13,0x92,0x13,0x4F,0x19,0xDE,0x1A,0x98,0x6D,0xA3,0xD4,
    0xC2,0x6F,0xCC,0xB6,0x4F,0xF1,0x3E,0xCA,0x09,0xC3,0x76,0xA4,0xB9,0x34,0xF5,0x41,
    0x67,0x78,0x3E,0xC5,0x9B,0x22,0xE1,0xC9,0x9B,0x2D,0x93,0x27,0x21,0x3A,0x55,0x0E,
    0xDF,0x3A,0x07,0xB4,0x52,0xC8,0x34,0x94,0x13,0x0F,0x8B,0x2E,0xAE,0x62,0x4B,0xA5,
    0xFC,0xD2,0x3C,0x41,0x53,0x62,0x8A,0xF7,0x26,0xD7,0xE4,0x23,0xF0,0x85,0xEA,0xBA,
    0x01,0x1E,0x88,0x18,0xEC,0xC9,0x45,0xC9,0xA1,0x03,0xCF,0x3A,0x1E,0xDC,0x82,0x1A,
    0xC5,0x99,0x93,0xC6,0x55,0xA8,0x06,0xDA,0xBB,0x29,0xDC,0x23,0x82,0xA6,0x5D,0x03,
    0x44,0xA6,0xF2,0xD9,0x4C,0xC0,0x32,0x82,0x41,0x9C,0xC8,0x86,0x58,0xDE,0xF3,0x53,
    0x6B,0xF7,0x5E,0x4B,0xFB,0x2F,0x81,0x3D,0x18,0xA0,0xBC,0xA3,0x6A,0x18,0x21,0xD3,
    0xC5,0xFA,0x93,0xCA,0xE6,0x02,0x8C,0x1D,0xE2,0xDC,0x8F,0x19,0xC6,0xB4,0xF9,0x91,
    0xF0,0x0E,0xEB,0x55,0xF1,0x8F,0x73,0x78,0x9D,0xFA,0x30,0x84,0x4D,0xAD,0x56,0x1C,
    0x39,0xA0,0x47,0x83,0x5F,0x7F,0x6B,0x67,0x5E,0xA3,0xCC,0xE8,0xA5,0xE9,0x8F,0x47,
    0x56,0x7A,0xA2,0x32,0x47,0xBB,0x94,0x47,0xD7,0x95,0xCD,0x20,0x2D,0x2F,0x07,0xF0,
    0xCA,0x14,0x87,0xE7,0xA9,0xE0,0x3D,0xED,0x02,0x03,0x01,0x00,0x01,0xA3,0x81,0xC9,
    0x30,0x81,0xC6,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x1D,0x04,
    0xFD,0x64,0x9B,0x64,0x95,0x70,0x6F,0x4C,0x0E,0x47,0x51,0xE7,0xBC,0xE2,0x28,0xC4,
    0xE9,0x43,0x30,0x0C,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x02,0x30,0x00,
    0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x2C,0xC5,0x6D,
    0x52,0xDD,0x31,0xEF,0x8C,0xEC,0x08,0x81,0xED,0xDF,0xDC,0xCA,0x43,0x00,0x45,0x01,
    0xD0,0x30,0x3C,0x06,0x03,0x55,0x1D,0x1F,0x04,0x35,0x30,0x33,0x30,0x31,0xA0,0x2F,
    0xA0,0x2D,0x86,0x2B,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x63,0x72,0x6C,0x2E,0x61,
    0x70,0x70,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x2F,0x61,0x70,0x70,0x6C,0x65,0x73,0x65,
    0x72,0x76,0x65,0x72,0x61,0x75,0x74,0x68,0x63,0x61,0x31,0x2E,0x63,0x72,0x6C,0x30,
    0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x07,0x80,0x30,
    0x16,0x06,0x03,0x55,0x1D,0x25,0x01,0x01,0xFF,0x04,0x0C,0x30,0x0A,0x06,0x08,0x2B,
    0x06,0x01,0x05,0x05,0x07,0x03,0x01,0x30,0x10,0x06,0x0A,0x2A,0x86,0x48,0x86,0xF7,
    0x63,0x64,0x06,0x1B,0x01,0x04,0x02,0x05,0x00,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,
    0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x0E,0x66,0xD7,
    0x3C,0x7B,0xB1,0x93,0xAB,0x83,0x2D,0x9F,0xC1,0x10,0x3E,0x78,0x1D,0x24,0x81,0x17,
    0x7E,0x94,0x83,0xA3,0x46,0x73,0xF4,0xD0,0xAA,0xA7,0x9F,0x3C,0xF8,0x18,0xCF,0x8C,
    0x50,0x11,0x67,0x94,0x55,0xD9,0x35,0x1A,0x35,0xA6,0x31,0xDD,0x51,0x18,0xD4,0x65,
    0xB7,0x7A,0x47,0xBB,0xC5,0xA5,0xC4,0xA5,0x5A,0x0D,0x20,0x76,0xF6,0xAB,0x2D,0x36,
    0x81,0xDA,0x43,0xE4,0xAC,0x5F,0xC5,0xA5,0x28,0x9F,0x1E,0x29,0x41,0x5D,0xD1,0x51,
    0x0F,0xD5,0x5B,0x28,0x87,0x20,0x0C,0x9F,0x2E,0x95,0xA1,0xFD,0x42,0xBD,0xAE,0x25,
    0x3F,0x53,0x92,0x95,0xD3,0xB4,0x49,0xBE,0xA8,0xB2,0x29,0x7F,0x99,0x35,0xEE,0x81,
    0x2A,0x4D,0xB5,0x00,0x64,0xCB,0xEA,0xBB,0x11,0xAC,0xBC,0x1C,0x04,0xF3,0x83,0x79,
    0x8D,0xBD,0x68,0x81,0xBF,0x25,0xBA,0x54,0xAB,0xC0,0x1E,0x75,0x8E,0x4E,0xE1,0xBF,
    0x29,0x1B,0xA7,0x2F,0xFD,0x91,0x64,0xEE,0xA0,0x96,0xD4,0xAB,0xED,0x6F,0x77,0x3A,
    0x87,0xE6,0x44,0xF6,0xAF,0xD2,0x7D,0x0D,0xBB,0x77,0x20,0xF4,0x1D,0x2E,0xA9,0x74,
    0x7B,0x8E,0xF6,0x34,0xA4,0x37,0x82,0x25,0x77,0x82,0x44,0x74,0xF9,0xAB,0xC6,0x62,
    0x1D,0xDF,0x34,0xBB,0x1C,0x09,0x30,0xD7,0x0F,0xAC,0xDA,0x9E,0x6F,0x30,0xAF,0xD6,
    0xC1,0x82,0x85,0x48,0xCB,0xBA,0xBE,0x6A,0x7E,0x44,0x32,0x43,0x65,0x48,0x5B,0x53,
    0x45,0xAD,0xC3,0x5D,0x22,0x89,0x40,0x36,0xE9,0xC4,0x86,0xFF,0x74,0x09,0x34,0x27,
    0x14,0x36,0x61,0x84,0x37,0x18,0xE2,0x9F,0xD5,0x69,0xE2,0x38,0xDF,
};


static void test_apple_server_auth_policy()
{
    CFDateRef date=NULL;
    CFArrayRef policies=NULL;
    SecPolicyRef policy=NULL;
    SecTrustRef trust=NULL;
    SecCertificateRef cert0=NULL, cert1=NULL;
    CFMutableArrayRef certs=NULL;
    SecTrustResultType trustResult;
    CFIndex chainLen;

    isnt(cert0 = SecCertificateCreateWithBytes(NULL, kTestAppleServerAuthLeafCert, sizeof(kTestAppleServerAuthLeafCert)),
         NULL, "create cert0");
    isnt(cert1 = SecCertificateCreateWithBytes(NULL, kTestAppleServerAuthCACert, sizeof(kTestAppleServerAuthCACert)),
         NULL, "create cert1");
    // these chain to the Apple Root CA so it is not provided
    
    isnt(certs = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks), NULL, "create cert array");
    CFArrayAppendValue(certs, cert0);
    CFArrayAppendValue(certs, cert1);

    /* Case 1: success */
    isnt(policy = SecPolicyCreateAppleSSLService(CFSTR("test.nosuchdomain")), NULL, "create policy");
    policies = CFArrayCreate(NULL, (const void **)&policy, 1, &kCFTypeArrayCallBacks);
    CFRelease(policy);
    policy = NULL;
    ok_status(SecTrustCreateWithCertificates(certs, policies, &trust), "create trust");
    CFRelease(policies);
    policies = NULL;
    isnt(date = CFDateCreateForGregorianZuluMoment(NULL, 2014, 7, 20, 12, 0, 0),
         NULL, "create verify date");
    ok_status(SecTrustSetVerifyDate(trust, date), "set date");
    CFReleaseSafe(date);
    date = NULL;
    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
    ok(trustResult == kSecTrustResultUnspecified, "trustResult 4 expected (got %d)",
       (int)trustResult);
    chainLen = SecTrustGetCertificateCount(trust);
    ok(chainLen == 3, "chain length 3 expected (got %d)", (int)chainLen);
    CFRelease(trust);
    trust = NULL;

    CFReleaseSafe(cert0);
    CFReleaseSafe(cert1);
    CFReleaseSafe(certs);
}

static void test_apple_corp_ca_policy()
{
    CFDateRef date=NULL;
    CFArrayRef policies=NULL;
    SecPolicyRef policy=NULL;
    SecTrustRef trust=NULL;
    SecCertificateRef cert0=NULL, cert1=NULL, cert2=NULL;
    CFMutableArrayRef certs=NULL, servers=NULL, anchors=NULL;
    
    SecTrustResultType trustResult;
    CFIndex chainLen;

    isnt(cert0 = SecCertificateCreateWithBytes(NULL, kTestAppleVPNLeafCert, sizeof(kTestAppleVPNLeafCert)),
         NULL, "create cert0");
    isnt(cert1 = SecCertificateCreateWithBytes(NULL, kTestAppleVPNCACert, sizeof(kTestAppleVPNCACert)),
         NULL, "create cert1");
    isnt(cert2 = SecCertificateCreateWithBytes(NULL, kTestAppleCorpCACert, sizeof(kTestAppleCorpCACert)),
         NULL, "create cert2");
    
    isnt(certs = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks), NULL, "create cert array");
    CFArrayAppendValue(certs, cert0);
    CFArrayAppendValue(certs, cert1);
    CFArrayAppendValue(certs, cert2);
    isnt(anchors = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks), NULL, "create anchors array");
    CFArrayAppendValue(anchors, cert2);

    /* Case 1: SSL server */
    isnt(policy = SecPolicyCreateSSL(true, CFSTR("com.apple.ist.ds.appleconnect2.production.vpn.8F2B3ADCD72ED2EA08DDC26AD0255A983B1DEBEB")), NULL, "create policy");
    policies = CFArrayCreate(NULL, (const void **)&policy, 1, &kCFTypeArrayCallBacks);
    CFRelease(policy);
    policy = NULL;
    ok_status(SecTrustCreateWithCertificates(certs, policies, &trust), "create trust");
    CFRelease(policies);
    policies = NULL;
    isnt(date = CFDateCreateForGregorianZuluMoment(NULL, 2014, 7, 20, 12, 0, 0),
         NULL, "create verify date");
    ok_status(SecTrustSetVerifyDate(trust, date), "set date");
    CFReleaseSafe(date);
    date = NULL;
    ok_status(SecTrustSetAnchorCertificates(trust, anchors), "set anchors");
    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
    ok(trustResult == kSecTrustResultRecoverableTrustFailure, "trustResult 5 expected (got %d)",
       (int)trustResult);
    chainLen = SecTrustGetCertificateCount(trust);
    ok(chainLen == 3, "chain length 3 expected (got %d)", (int)chainLen);
    CFRelease(trust);
    trust = NULL;

    /* Case 2: SSL client */
    isnt(policy = SecPolicyCreateSSL(false, NULL), NULL, "create policy");
    policies = CFArrayCreate(NULL, (const void **)&policy, 1, &kCFTypeArrayCallBacks);
    CFRelease(policy);
    policy = NULL;
    ok_status(SecTrustCreateWithCertificates(certs, policies, &trust), "create trust");
    CFRelease(policies);
    policies = NULL;
    isnt(date = CFDateCreateForGregorianZuluMoment(NULL, 2014, 7, 20, 12, 0, 0),
         NULL, "create verify date");
    ok_status(SecTrustSetVerifyDate(trust, date), "set date");
    CFReleaseSafe(date);
    date = NULL;
    ok_status(SecTrustSetAnchorCertificates(trust, anchors), "set anchors");
    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
    ok(trustResult == kSecTrustResultUnspecified, "trustResult 4 expected (got %d)",
       (int)trustResult);
    chainLen = SecTrustGetCertificateCount(trust);
    ok(chainLen == 3, "chain length 3 expected (got %d)", (int)chainLen);
    CFRelease(trust);
    trust = NULL;

    /* Case 3: EAP */
    isnt(servers = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks), NULL, "create cert array");
    CFArrayAppendValue(servers, CFSTR("com.apple.ist.ds.appleconnect2.production.vpn.8F2B3ADCD72ED2EA08DDC26AD0255A983B1DEBEB"));
    isnt(policy = SecPolicyCreateEAP(true, servers), NULL, "create policy");
    policies = CFArrayCreate(NULL, (const void **)&policy, 1, &kCFTypeArrayCallBacks);
    CFRelease(policy);
    policy = NULL;
    ok_status(SecTrustCreateWithCertificates(certs, policies, &trust), "create trust");
    CFRelease(policies);
    policies = NULL;
    isnt(date = CFDateCreateForGregorianZuluMoment(NULL, 2014, 7, 20, 12, 0, 0),
         NULL, "create verify date");
    ok_status(SecTrustSetVerifyDate(trust, date), "set date");
    CFReleaseSafe(date);
    date = NULL;
    ok_status(SecTrustSetAnchorCertificates(trust, anchors), "set anchors");
    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
    ok(trustResult == kSecTrustResultUnspecified, "trustResult 4 expected (got %d)",
       (int)trustResult);
    chainLen = SecTrustGetCertificateCount(trust);
    ok(chainLen == 3, "chain length 3 expected (got %d)", (int)chainLen);
    CFRelease(trust);
    trust = NULL;

    /* Case 4: IPsec */
    isnt(policy = SecPolicyCreateIPSec(false, NULL), NULL, "create policy");
    policies = CFArrayCreate(NULL, (const void **)&policy, 1, &kCFTypeArrayCallBacks);
    CFRelease(policy);
    policy = NULL;
    ok_status(SecTrustCreateWithCertificates(certs, policies, &trust), "create trust");
    CFRelease(policies);
    policies = NULL;
    isnt(date = CFDateCreateForGregorianZuluMoment(NULL, 2014, 7, 20, 12, 0, 0),
         NULL, "create verify date");
    ok_status(SecTrustSetVerifyDate(trust, date), "set date");
    CFReleaseSafe(date);
    date = NULL;
    ok_status(SecTrustSetAnchorCertificates(trust, anchors), "set anchors");
    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate trust");
    ok(trustResult == kSecTrustResultUnspecified, "trustResult 4 expected (got %d)",
       (int)trustResult);
    chainLen = SecTrustGetCertificateCount(trust);
    ok(chainLen == 3, "chain length 3 expected (got %d)", (int)chainLen);
    CFRelease(trust);
    trust = NULL;

    CFReleaseSafe(cert0);
    CFReleaseSafe(cert1);
    CFReleaseSafe(cert2);
    CFReleaseSafe(certs);
    CFReleaseSafe(servers);
    CFReleaseSafe(anchors);
}

static void tests(void)
{
    test_apple_server_auth_policy();
    test_apple_corp_ca_policy();
}

int si_81_sectrust_server_auth(int argc, char *const *argv)
{
	plan_tests(48);

	tests();

	return 0;
}
