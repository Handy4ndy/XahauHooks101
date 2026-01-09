//**************************************************************
// Xahau Hook 101 Example ~ Invoke URI Remit
// Author: @Handy_4ndy
//
// Description:
//   This hook mints numbered URITokens on incoming payments using a
//   sequential numbering system. Hook owner uses Invoke to set the URI prefix
//   and total count. Each payment mints the next URIToken until depleted.
//
// Parameters (set via Invoke):
//   'PREFIX' (variable length): The URI prefix (e.g., "ipfs://hash/")
//   'COUNT' (8 bytes): Number of NFTs available to mint (uint64_t)
//
// Key Benefits of Sequential Minting:
//   ✅ Automatic sequential numbering (000001, 000002, etc.)
//   ✅ Dynamic configuration via Invoke
//   ✅ Controlled NFT supply management
//   ✅ Professional .json suffix for metadata
//
// Usage:
//   - Owner sends Invoke with PREFIX and COUNT to configure the hook.
//   - Incoming payments mint sequential URITokens until COUNT reaches zero.
//   - Each URIToken follows format: {PREFIX}{000001-999999}.json
//   - [Hooks Services](https://hooks.services/tools)
//
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

// Prepare Remit macro (without etxn_reserve - call separately)
#define PREPARE_REMIT_TXN(hook_acc, dest_acc, uri, uri_len) do { \
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
    TRACESTR("IUR :: Invoke URI Remit :: Called.");

    // Get Hook account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // Get origin tx account
    uint8_t otx_acc[20];
    otxn_field(otx_acc, 20, sfAccount);

    // Get tx type
    int64_t tt = otxn_type();

    // Check if Hook Account and Transaction Account are the same
    int equal = 0;
    BUFFER_EQUAL(equal, otx_acc, hook_acc, 20);

    // INVOKE TRANSACTION (tt=99) - Only hook owner can configure
    if (tt == 99) {
        if (!equal) {
            rollback(SBUF("IUR :: Error :: Only hook owner can configure!"), __LINE__);
        }

        TRACESTR("Processing invoke transaction...");

        // State keys
        uint8_t prefix_key[6] = {'P', 'R', 'E', 'F', 'I', 'X'};
        uint8_t count_key[5] = {'C', 'O', 'U', 'N', 'T'};
        uint8_t total_key[5] = {'T', 'O', 'T', 'A', 'L'};

        // Check for PREFIX parameter
        uint8_t prefix_buffer[256];
        int64_t prefix_len = otxn_param(SBUF(prefix_buffer), SBUF(prefix_key));

        if (prefix_len > 0) {
            TRACESTR("Setting PREFIX state...");
            TRACEVAR(prefix_len);
            TRACEHEX(prefix_buffer);

            if (state_set(prefix_buffer, prefix_len, SBUF(prefix_key)) < 0) {
                rollback(SBUF("IUR :: Error :: Could not set PREFIX state!"), __LINE__);
            }
        }

        // Check for COUNT parameter
        uint8_t count_param_buf[8];
        int64_t count_param_len = otxn_param(SBUF(count_param_buf), SBUF(count_key));

        if (count_param_len == 8) {
            uint64_t count_value = UINT64_FROM_BUF(count_param_buf);
      
            // Set COUNT state
            if (state_set(SBUF(count_param_buf), SBUF(count_key)) < 0) {
                rollback(SBUF("IUR :: Error :: Could not set COUNT state!"), __LINE__);
            }

            // Also set TOTAL state (for numbering sequence)
            if (state_set(SBUF(count_param_buf), SBUF(total_key)) < 0) {
                rollback(SBUF("IUR :: Error :: Could not set TOTAL state!"), __LINE__);
            }

            TRACESTR("COUNT and TOTAL states set successfully.");
        }

        accept(SBUF("IUR :: Configuration updated successfully!"), __LINE__);
    }

    // PAYMENT TRANSACTION (tt=0) - Mint URIToken
    if (tt == 0) {
      
        // State keys
        uint8_t prefix_key[6] = {'P', 'R', 'E', 'F', 'I', 'X'};
        uint8_t count_key[5] = {'C', 'O', 'U', 'N', 'T'};
        uint8_t total_key[5] = {'T', 'O', 'T', 'A', 'L'};

        // Check if configuration exists
        uint8_t prefix_buffer[256];
        int64_t prefix_len = state(SBUF(prefix_buffer), SBUF(prefix_key));

        if (prefix_len <= 0) {
            rollback(SBUF("IUR :: Error :: No URI prefix configured! Use invoke to set PREFIX."), __LINE__);
        }

        uint8_t count_buf[8];
        if (state(SBUF(count_buf), SBUF(count_key)) < 0) {
            rollback(SBUF("IUR :: Error :: No count configured! Use invoke to set COUNT."), __LINE__);
        }

        uint64_t remaining_count = UINT64_FROM_BUF(count_buf);
        TRACEVAR(remaining_count);

        // Check if any NFTs remain
        if (remaining_count <= 0) {
            rollback(SBUF("IUR :: Error :: No NFTs remaining to mint!"), __LINE__);
        }

        // Get total count for sequence numbering
        uint8_t total_buf[8];
        if (state(SBUF(total_buf), SBUF(total_key)) < 0) {
            rollback(SBUF("IUR :: Error :: Could not get total count!"), __LINE__);
        }

        uint64_t total_count = UINT64_FROM_BUF(total_buf);
        uint64_t current_number = total_count - remaining_count + 1;

        TRACEVAR(total_count);
        TRACEVAR(current_number);

        // Build the full URI: prefix + padded_number + ".json"
        uint8_t full_uri[300];
        int uri_pos = 0;

        // Copy prefix
        for (int i = 0; GUARD(256), i < prefix_len; ++i) {
            full_uri[uri_pos++] = prefix_buffer[i];
        }

        // Add zero-padded number (6 digits: 000001)
        uint8_t number_str[7]; // 6 digits + null terminator space
        
        // Convert number to 6-digit zero-padded string
        number_str[0] = '0' + ((current_number / 100000) % 10);
        number_str[1] = '0' + ((current_number / 10000) % 10);
        number_str[2] = '0' + ((current_number / 1000) % 10);
        number_str[3] = '0' + ((current_number / 100) % 10);
        number_str[4] = '0' + ((current_number / 10) % 10);
        number_str[5] = '0' + (current_number % 10);

        // Append number
        for (int i = 0; GUARD(6), i < 6; ++i) {
            full_uri[uri_pos++] = number_str[i];
        }

        // Append ".json"
        uint8_t json_suffix[5] = {'.', 'j', 's', 'o', 'n'};
        for (int i = 0; GUARD(5), i < 5; ++i) {
            full_uri[uri_pos++] = json_suffix[i];
        }

        uint64_t final_uri_len = uri_pos;

        // Reserve space for emission
        etxn_reserve(1);

        // Prepare and emit the transaction
        PREPARE_REMIT_TXN(hook_acc, otx_acc, full_uri, final_uri_len);

        uint8_t emithash[32];
        int64_t emit_result = emit(SBUF(emithash), txn, BYTES_LEN + final_uri_len + 1);

        if (emit_result > 0) {
            // Decrement count
            remaining_count--;
            UINT64_TO_BUF(count_buf, remaining_count);

            if (state_set(SBUF(count_buf), SBUF(count_key)) < 0) {
                rollback(SBUF("IUR :: Error :: Could not update count state!"), __LINE__);
            }

            TRACESTR("Sequential URIToken minted successfully!");
            TRACEVAR(remaining_count);

            accept(SBUF("IUR :: Success :: URIToken minted!"), __LINE__);
        }

        rollback(SBUF("IUR :: Error :: Failed to mint URIToken!"), __LINE__);
    }

    // Accept other transaction types
    accept(SBUF("IUR :: Transaction accepted."), __LINE__);

    _g(1,1);  // Guard against compiler removing this
    return 0;
}