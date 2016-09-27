/*
 * Copyright (c) 2006-2008,2010 Apple Inc. All Rights Reserved.
 */

#include <Security/vmdh.h>
#include <Security/SecBase64.h>
#include <stdlib.h>
#include <unistd.h>

#include <corecrypto/ccdh.h>

#include "Security_regressions.h"

#if 0
#include <openssl/dh.h>
#endif

/* How to reach in the internals of SecDH/vmdh struct */
static inline ccdh_gp_t vmdh_gp(struct vmdh *dh)
{
    void *p = dh;
    ccdh_gp_t gp = { .gp = p };
    return gp;
}

static inline ccdh_full_ctx_t vmdh_priv(struct vmdh *dh)
{
    void *p = dh;
    cczp_t zp = { .u = p };
    cc_size s = ccn_sizeof_n(cczp_n(zp));
    ccdh_full_ctx_t priv = { .hdr = (struct ccdh_ctx_header *)(p+ccdh_gp_size(s)) };
    return priv;
}

static uint32_t param_g = 5;

static const uint8_t param_p[] = {
    0xD7, 0x48, 0x26, 0xCE, 0x1B, 0x1E, 0x26, 0xEE,
    0x2B, 0x03, 0xAA, 0xAF, 0x3E, 0x1A, 0x82, 0x39,
    0x43, 0x2E, 0xB4, 0x36, 0x71, 0xF7, 0x5A, 0x6B,
    0x62, 0x8D, 0x40, 0xFC, 0x39, 0xB8, 0x85, 0xB5,
    0xA2, 0x50, 0x56, 0xE3, 0xCB, 0xF1, 0xE4, 0xFE,
    0xA8, 0x64, 0x63, 0xA0, 0x6F, 0x19, 0xF4, 0xB0,
    0x12, 0x11, 0x82, 0x02, 0xBB, 0xBD, 0xFF, 0xA5,
    0x8C, 0xED, 0xBF, 0xBE, 0xF0, 0xF6, 0x18, 0xDB,
};

static const uint8_t client_priv[] = {
    0x48, 0x52, 0x51, 0x7B, 0x04, 0xF6, 0x8B, 0xF4,
    0x25, 0x9D, 0x08, 0x9F, 0x22, 0xB2, 0xDC, 0x6D,
    0x84, 0x36, 0x1B, 0xBB, 0xC9, 0xEE, 0x91, 0x2F,
    0x24, 0xFD, 0xF5, 0xEF, 0xC4, 0x8B, 0xAB, 0x39,
    0x77, 0xE5, 0x6B, 0x31, 0x82, 0x96, 0x3C, 0x6E,
    0xDC, 0x0A, 0x40, 0x39, 0x8F, 0xF3, 0x4A, 0x63,
    0x00, 0x39, 0x8C, 0x52, 0x01, 0xB9, 0x48, 0xF4,
    0xB1, 0xD2, 0x3B, 0x24, 0xF3, 0x30, 0x0B, 0x61,
};

static const uint8_t client_pub[] = {
    0x73, 0x00, 0x67, 0x2E, 0x71, 0x38, 0x55, 0x6A,
    0x9C, 0xA4, 0x1E, 0x7A, 0x53, 0x3A, 0x22, 0xD8,
    0xF0, 0x37, 0xBD, 0x57, 0x9F, 0x4A, 0xD1, 0xD1,
    0x16, 0x2F, 0x23, 0xB7, 0x3C, 0xBC, 0x15, 0x6A,
    0x1D, 0x38, 0xE2, 0x2D, 0x4E, 0xCB, 0x8E, 0x9C,
    0xFE, 0x86, 0x2A, 0xCA, 0xA5, 0xA6, 0xE6, 0x4E,
    0x67, 0x2A, 0x0C, 0x37, 0x06, 0x95, 0x5B, 0xF5,
    0xA8, 0x69, 0xBE, 0x72, 0x54, 0x0C, 0x63, 0xDD,
};

static const uint8_t server_priv[] = {
    0x7A, 0x20, 0x70, 0x42, 0xF2, 0x19, 0x0F, 0x54,
    0xA4, 0xD6, 0x45, 0x85, 0xFF, 0xE3, 0x4F, 0x95,
    0x77, 0x4E, 0x05, 0xA9, 0xCF, 0x60, 0xFA, 0x54,
    0xEC, 0xBC, 0x8B, 0x67, 0xFD, 0x59, 0x12, 0xEF,
    0xC0, 0xD3, 0x75, 0x66, 0x3F, 0xE4, 0x50, 0x97,
    0x94, 0x12, 0xB4, 0xE3, 0xB2, 0x5E, 0x69, 0x90,
    0x18, 0x56, 0xAF, 0x50, 0x2D, 0xB9, 0xC9, 0x11,
    0x8B, 0xF7, 0x75, 0x70, 0xDF, 0x16, 0xCB, 0x8F,
};

static const uint8_t server_pub[] = {
    0x36, 0x21, 0x01, 0x4B, 0x8C, 0xD4, 0xFD, 0xA3,
    0xBA, 0x12, 0x7B, 0xD7, 0x48, 0x83, 0x74, 0x09,
    0x74, 0x46, 0xAA, 0xE0, 0x22, 0x1B, 0x2F, 0x25,
    0xE7, 0x25, 0x25, 0x4B, 0x7E, 0xAE, 0xEA, 0x30,
    0xE0, 0xFD, 0xC8, 0x5A, 0xD5, 0xD2, 0x9C, 0x54,
    0xCE, 0x0E, 0x8A, 0xB1, 0x16, 0xD8, 0xA3, 0xFA,
    0xD9, 0x85, 0xA8, 0x42, 0x4E, 0xF5, 0x77, 0x9F,
    0x2D, 0xAB, 0x4C, 0xE2, 0x1C, 0xE7, 0xCE, 0xD9,
};

static const uint8_t pw[] = {
	0x31, 0x32, 0x33, 0x34
};

/* base64 would be:
rHWs3KX/1ubFTzfim+9mLw==
*/
static const uint8_t pw_encr[] = {
	0xac, 0x75, 0xac, 0xdc, 0xa5, 0xff, 0xd6, 0xe6,
	0xc5, 0x4f, 0x37, 0xe2, 0x9b, 0xef, 0x66, 0x2f
};

#if 0
static void hexdump(const uint8_t *bytes, size_t len) {
	size_t ix;
	for (ix = 0; ix < len; ++ix) {
		printf("%02X", bytes[ix]);
	}
	printf("\n");
}
#endif

/* Test basic add delete update copy matching stuff. */
static void tests(void)
{
    vmdh_t vmdh;
    ok((vmdh = vmdh_create(param_g, param_p, sizeof(param_p),
        NULL, 0)), "vmdh_create");

    /* No SecDH API to import a private key, so we have to reach inside the opaque struct */
    ccdh_gp_t gp = vmdh_gp(vmdh);
    ccdh_full_ctx_t priv = vmdh_priv(vmdh);
    ok_status(ccdh_import_priv(gp, sizeof(client_priv), client_priv, priv), "Import private DH key");

    uint8_t encpw[vmdh_encpw_len(sizeof(pw))];
    size_t encpw_len = sizeof(encpw);

    ok(vmdh_encrypt_password(vmdh, server_pub, sizeof(server_pub),
		pw, sizeof(pw),	encpw, &encpw_len), "vmdh_encrypt_password");
    vmdh_destroy(vmdh);

    is(encpw_len, sizeof(pw_encr), "check ciphertext size");
	ok(!memcmp(encpw, pw_encr, sizeof(pw_encr)), "does ciphertext match?");

#if 0
	hexdump(encpw, encpw_len);
	hexdump(pw_encr, sizeof(pw_encr));

	DH *dh = DH_new();
	dh->p = BN_bin2bn(param_p, sizeof(param_p), BN_new());
	BN_hex2bn(&dh->g, "05");
	dh->pub_key = BN_bin2bn(client_pub, sizeof(client_pub), BN_new());
	dh->priv_key = BN_bin2bn(client_priv, sizeof(client_priv), BN_new());
	BIGNUM *bn_server_pub = BN_bin2bn(server_pub, sizeof(server_pub), BN_new());
	unsigned char key[1024];
	DH_compute_key(key, bn_server_pub, dh);
	printf("shared secret:\n");
	hexdump(key, 16);
#endif
}

int vmdh_41_example(int argc, char *const *argv)
{
	plan_tests(5);

	tests();

	return 0;
}
