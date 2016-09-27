//
//  OTRMathPrivate.h
//  libsecurity_libSecOTR
//
//  Created by Mitch Adler on 1/28/11.
//  Copyright 2011 Apple Inc. All rights reserved.
//

#ifndef _OTRMATHPRIVATE_H_
#define _OTRMATHPRIVATE_H_

#include <Security/SecOTRMath.h>

static const cc_unit kOTRGeneratorValue = 2;

#define kOTRDHGroupUnits ccn_nof_size(sizeof(kOTRDHGroup))
static const uint8_t kOTRDHGroup[] = {
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xC9, 0x0F, 0xDA, 0xA2,
    0x21, 0x68, 0xC2, 0x34,
    0xC4, 0xC6, 0x62, 0x8B,
    0x80, 0xDC, 0x1C, 0xD1,
    0x29, 0x02, 0x4E, 0x08,
    0x8A, 0x67, 0xCC, 0x74,
    0x02, 0x0B, 0xBE, 0xA6,
    0x3B, 0x13, 0x9B, 0x22,
    0x51, 0x4A, 0x08, 0x79,
    0x8E, 0x34, 0x04, 0xDD,
    0xEF, 0x95, 0x19, 0xB3,
    0xCD, 0x3A, 0x43, 0x1B,
    0x30, 0x2B, 0x0A, 0x6D,
    0xF2, 0x5F, 0x14, 0x37,
    0x4F, 0xE1, 0x35, 0x6D,
    0x6D, 0x51, 0xC2, 0x45,
    0xE4, 0x85, 0xB5, 0x76,
    0x62, 0x5E, 0x7E, 0xC6,
    0xF4, 0x4C, 0x42, 0xE9,
    0xA6, 0x37, 0xED, 0x6B,
    0x0B, 0xFF, 0x5C, 0xB6,
    0xF4, 0x06, 0xB7, 0xED,
    0xEE, 0x38, 0x6B, 0xFB,
    0x5A, 0x89, 0x9F, 0xA5,
    0xAE, 0x9F, 0x24, 0x11,
    0x7C, 0x4B, 0x1F, 0xE6,
    0x49, 0x28, 0x66, 0x51,
    0xEC, 0xE4, 0x5B, 0x3D,
    0xC2, 0x00, 0x7C, 0xB8,
    0xA1, 0x63, 0xBF, 0x05,
    0x98, 0xDA, 0x48, 0x36,
    0x1C, 0x55, 0xD3, 0x9A,
    0x69, 0x16, 0x3F, 0xA8,
    0xFD, 0x24, 0xCF, 0x5F,
    0x83, 0x65, 0x5D, 0x23,
    0xDC, 0xA3, 0xAD, 0x96,
    0x1C, 0x62, 0xF3, 0x56,
    0x20, 0x85, 0x52, 0xBB,
    0x9E, 0xD5, 0x29, 0x07,
    0x70, 0x96, 0x96, 0x6D,
    0x67, 0x0C, 0x35, 0x4E,
    0x4A, 0xBC, 0x98, 0x04,
    0xF1, 0x74, 0x6C, 0x08,
    0xCA, 0x23, 0x73, 0x27,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

#define kOTRDHGroupRecipUnits ccn_nof_size(sizeof(kOTRDHGroup_Recip))
static const uint8_t kOTRDHGroup_Recip[] = {
    0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x36, 0xF0, 0x25,
    0x5D, 0xDE, 0x97, 0x3D,
    0xCB, 0x47, 0x03, 0xCE,
    0x7E, 0x2E, 0x81, 0x51,
    0x97, 0xA6, 0xDB, 0x0F,
    0x58, 0x84, 0x48, 0xB6,
    0x11, 0x64, 0xCF, 0xCA,
    0xC5, 0xF1, 0x87, 0x2E,
    0x51, 0xB1, 0xF9, 0xFB,
    0xB5, 0xBF, 0x16, 0xFB,
    0xE7, 0x96, 0x89, 0xFC,
    0x09, 0x03, 0xA8, 0x01,
    0xE3, 0xD4, 0x80, 0x2F,
    0xB8, 0xD3, 0x29, 0x55,
    0x0D, 0xC8, 0xC9, 0xD3,
    0xD9, 0x22, 0xEE, 0xCE,
    0x9A, 0x54, 0x75, 0xDB,
    0x33, 0xDB, 0x7B, 0x83,
    0xBB, 0x5C, 0x0E, 0x13,
    0xD1, 0x68, 0x04, 0x9B,
    0xBC, 0x86, 0xC5, 0x81,
    0x76, 0x47, 0xB0, 0x88,
    0xD1, 0x7A, 0xA5, 0xCC,
    0x40, 0xE0, 0x20, 0x35,
    0x58, 0x8E, 0xDB, 0x2D,
    0xE1, 0x89, 0x93, 0x41,
    0x37, 0x19, 0xFC, 0x25,
    0x8D, 0x79, 0xBC, 0x21,
    0x7A, 0xC4, 0xB8, 0x73,
    0x9C, 0xBE, 0xA0, 0x38,
    0xAA, 0xA8, 0x8D, 0x0D,
    0x2F, 0x78, 0xA7, 0x7A,
    0x8A, 0x6F, 0xC7, 0xFA,
    0xA8, 0xB2, 0xBD, 0xCA,
    0x9B, 0xE7, 0x50, 0x2D,
    0x2F, 0x5F, 0x6A, 0x7B,
    0x65, 0xF5, 0xE4, 0xF0,
    0x7A, 0xB8, 0xB2, 0x86,
    0xE4, 0x11, 0x15, 0xF0,
    0x24, 0xA6, 0xE9, 0x76,
    0xBD, 0x2B, 0xCE, 0x3E,
    0x51, 0x90, 0xB8, 0x91,
    0xAB, 0xBF, 0x23, 0x31,
    0xE9, 0xC9, 0x42, 0x97,
    0x73, 0xF1, 0x15, 0xD2,
    0x7D, 0x32, 0xC6, 0x95,
    0xE0
};

#endif
