# XahauHooks101 — verification findings

An independent verification pass over all 108 hooks: each was compiled, statically reviewed for the
usual Hook safety classes (return-code discipline, XFL vs native amount math, emit budget + destination,
state bounds, guard coverage, intent-vs-implementation), then **every flagged issue was put through an
adversarial second pass that tried to refute it.** Only issues that survived refutation are listed.

It's a clean library — low real-defect rate for 108 teaching hooks, most compile lint-clean and do what
their headers say. The items below are a short, tractable fix list.

> Note: the `copilot/review-iou-emit-remit` branch already refactors the `Emit_*` / `Remit_*` families,
> so those are intentionally excluded here to avoid conflicting with that work. The `Emit_IOU`
> wrong-variable note (last section) is included only because it's the same area, for cross-check.

## Compile breakdown
| Outcome | Count |
|---|---:|
| Lint-clean WASM (xahc) | 62 |
| Compiles with `-Wno-int-conversion` (pointer-as-uint32, valid on wasm32 — the Hooks Builder permits it; newer clang errors) | 42 |
| Hard compile failure (real source defect) | 4 |

## Confirmed correctness issues

### Compile failures (deploy-blocking)
| Hook | Defect | Fix |
|---|---|---|
| `Basic_State/basic_state_add.c` | `hook_account(hook_acct,20)`, `otxn_field(otx_acc,20,sfAccount)`, `state_set(val_buf,val_len,key_buf,key_len)` pass raw arrays where the host fn wants a `uint32_t` ptr — clang hard-errors, no WASM. | Use the `SBUF()` form consistently, matching the working hooks in the same family. |
| `Basic_State/basic_state_counter.c` | Same array-arg pattern on `hook_account` + `otxn_field`. | `SBUF()` form (the `state_set` calls in the file already use it). |
| `Basic_State/basic_state_manager.c` | Same pattern on `hook_account` / `otxn_field` / three `state_set` calls. | `SBUF()` form on all buffer args. |
| `Basic_State/basic_state_delete.c` | Same pattern — **plus** a logic defect (below). | `SBUF()` form, then fix the counter. |
| `Basic_State/basic_state_count_increase.c` | `TRAVCEVAR(count)` — typo, undeclared identifier, clang aborts. | `TRAVCEVAR` → `TRACEVAR`. |

### Logic / correctness
| Hook | Defect | Fix |
|---|---|---|
| `Basic_State/basic_state_delete.c` | `count--` runs unconditionally on any 8-byte `DEL`, even when `count==0` or the key never existed (delete of an absent key returns ≥0) → `uint64_t` underflows to `0xFFFF…`. | Only decrement when a key was actually present and `count > 0`; rollback on delete-of-missing. |
| `Basic_IOU/Reject_Outgoing_Payment/reject_outgoing_iou_multi.c` | `cost_drops = cost_iou * 100000` (10⁵) but `amount_drops` is scaled by 10⁶, so the "multiple of 10 IOU" rule actually enforces "multiple of 1." The three sibling files use `* 1000000`. | `100000` → `1000000`. |
| `Basic_Native/Reject_Outgoing_Payment/reject_outgoing_max.c` | Header documents a 10 XAH cap in five places; code sets `max_amount = 100` → enforces 100 XAH (10× the lesson). | `max_amount = 10` (or correct the header to 100). |
| `Basic_Install_Parameters/install_parameter_exact.c` | `hook_param` key passed as a bare string literal (build failure); exact-amount gate uses native `double` equality instead of the XFL it already computes. | Fix the `hook_param` key arg; decide via `float_compare` on the XFL. |
| `Basic_Install_Parameters/install_parameter_max.c`, `…/install_parameter_min.c` | Duplicate local `otxn_drops` (redefinition → no build); threshold compared with native `double` instead of the computed XFL. | Remove the duplicate `otxn_drops`; route the threshold through `float_compare`. |

### Refuted on the adversarial pass (NOT bugs — for the record)
- "Threshold bypass via `float_int` overflow" — refuted across the emit/remit hooks.
- "`float_sto(amt_out - 1, …)` out-of-bounds" — refuted; that's the canonical Xahau IOU-emit idiom.

### Cross-check (Emit_IOU — area under refactor, not changed here)
`Emit_IOU/{install_iou_emit, hardcoded_multi_iou_emit, install_multi_iou_emit, invoke_iou_emit}.c`
compute `iou_amount_xfl = float_set(0, amountOut)` (the configured `AMT_OUT`) but `float_sto(...)`
serializes `amount_xfl` (the incoming amount) instead, so the emitted IOU pays the received amount and
`AMT_OUT` is inert. Worth confirming the `copilot/review-iou-emit-remit` refactor passes `iou_amount_xfl`
to `float_sto`.

## Prove roadmap (optional next step)
The `Emit_*` / `Remit_*` / `Basic_State` hooks are good formal-proof targets — e.g. "emits at most one
payment," "can't pay more than `AMT_OUT`," "state counter stays monotonic." ~30 hooks could carry a real
proof via `xahc-prover`. Happy to pick one and work it through.
