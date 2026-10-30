//**************************************************************
// Xahau Hook 101 Example ~ Install URI Remit
// Author: @Handy_4ndy
//
// Description:
//   Mints a URIToken on incoming payments using a URI defined at installation time. Simple, fixed URI minting for consistent NFT creation with predefined metadata.
//
// Triggers:
//   ttPAYMENT (incoming)
//
// Parameters (set at Install):
//   'URI' (variable length): The URI for the token to be minted
//
// Usage:
//   - Set URI parameter during hook installation.
//   - Incoming payments automatically mint URITokens with the fixed URI.
//   - Perfect for single-collection NFT projects.
//   - [Hooks Services](https://hooks.services/tools)
//
// Accepts:
//   - Incoming payments (mints URITokens)
//   - Outgoing payments
//
// Rejects:
//   - Payments if required URI is missing or invalid
//**************************************************************
#include "hookapi.h"

// Macro to copy account ID to buffer
#define ACCOUNT_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint32_t*)(buf + 16) = *(uint32_t*)(i + 16);\
}

// Fixed macro to set vlURI (len, data, end marker 0xE1) - byte-by-byte, no overrun
#define URI_TO_BUF(buf_raw, uri, len)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    buf[0] = len;\
    for (int i = 0; GUARD(512), i < len; ++i) \
        buf[1 + i] = uri[i]; \
    buf[1 + len] = 0xE1U; \
}

// Remit tx skeleton (clang-format off)
uint8_t txn[1000] =
{
/* size,upto */
/*   3,   0 */   0x12U, 0x00U, 0x5FU,                                                           /* tt = Remit       */
/*   5,   3 */   0x22U, 0x80U, 0x00U, 0x00U, 0x00U,                                          /* flags = tfCanonical */
/*   5,   8 */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,                                                 /* sequence = 0 */
/*   5,  13 */   0x99U, 0x99U, 0x99U, 0x99U, 0x99U,                                                /* dtag, flipped */
/*   6,  18 */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,                                      /* first ledger seq */
/*   6,  24 */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,                                       /* last ledger seq */
/*   9,  30 */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,                         /* fee      */
/*  35,  39 */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* pubkey   */
/*  22,  74 */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                  /* srcacc  */
/*  22,  96 */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                  /* dstacc  */
/* 116, 118 */   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    /* emit detail */
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

/*   3, 234 */  0xE0U, 0x5CU, 0x75U,
/*   1, 237 */  0xE1U,
/*   0, 238 */
};
// clang-format on

// Tx builder defines
#define BYTES_LEN 238U
#define FLS_OUT (txn + 20U)
#define LLS_OUT (txn + 26U)
#define DTAG_OUT (txn + 14U)
#define FEE_OUT (txn + 31U)
#define HOOK_ACC (txn + 76U)
#define OTX_ACC (txn + 98U)
#define URI_OUT (txn + 237U)
#define EMIT_OUT (txn + 118U)

// Prepare Remit macro
#define PREPARE_REMIT_TXN(hook_acc, dest_acc, uri, uri_len) do { \
    etxn_reserve(1); \
    if (otxn_field(DTAG_OUT, 4, sfSourceTag) == 4) \
        *(DTAG_OUT - 1) = 0x2EU; \
    uint32_t fls = (uint32_t)ledger_seq() + 1; \
    *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN_32(fls); \
    uint32_t lls = fls + 4; \
    *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN_32(lls); \
    ACCOUNT_TO_BUF(HOOK_ACC, hook_acc); \
    ACCOUNT_TO_BUF(OTX_ACC, dest_acc); \
    URI_TO_BUF(URI_OUT, uri, uri_len); \
    etxn_details(EMIT_OUT, 116U); \
    int64_t fee = etxn_fee_base(txn, BYTES_LEN + uri_len + 1); \
    uint8_t *b = FEE_OUT; \
    *b++ = 0b01000000 + ((fee >> 56) & 0b00111111); \
    *b++ = (fee >> 48) & 0xFFU; \
    *b++ = (fee >> 40) & 0xFFU; \
    *b++ = (fee >> 32) & 0xFFU; \
    *b++ = (fee >> 24) & 0xFFU; \
    *b++ = (fee >> 16) & 0xFFU; \
    *b++ = (fee >> 8) & 0xFFU; \
    *b++ = (fee >> 0) & 0xFFU; \
} while(0)

int64_t hook(uint32_t reserved) {
    TRACESTR("IUR :: Install URI Remit :: Called.");

    // Get Hook account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // Get origin tx account (payer)
    uint8_t otx_acc[20];
    otxn_field(otx_acc, 20, sfAccount);

    // Get tx type
    int64_t tt = otxn_type();

    // Only process Payments
    if (tt != 0) {
        accept(SBUF("IUR :: Not a Payment. Accepting."), __LINE__);
    }

    // Get URI from install parameters
    uint8_t uri[512];  // Buffer for URI (up to 512 bytes)
    int64_t uri_len = hook_param(SBUF(uri), "URI", 3);
    
    if (uri_len <= 0) {
        rollback(SBUF("IUR :: Error :: URI parameter not set or invalid."), __LINE__);
    }
    
    if (uri_len > 511) {
        rollback(SBUF("IUR :: Error :: URI too long (max 511 bytes)."), __LINE__);
    }

    // Prepare Remit tx
    PREPARE_REMIT_TXN(hook_acc, otx_acc, uri, uri_len);

    // Emit the tx
    uint8_t emithash[32];
    int64_t emit_result = emit(SBUF(emithash), txn, BYTES_LEN + uri_len + 1);

    if (emit_result > 0) {
        accept(SBUF("IUR :: Success :: URIToken minted!"), __LINE__);
    }

    rollback(SBUF("IUR :: Error :: Emit failed."), __LINE__);

    _g(1,1);  // Guard against compiler removing this
    return 0;
}