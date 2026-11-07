# Traceability — refactor-guide-v2.md

Failure types (FT1–FT12) are defined in `phase1-failure-analysis.md` §2 (Phase 1 repo, tag `phase1-complete`). Code examples come from the commits listed there. FT IDs and rule IDs are unchanged from v1 so versions stay comparable.

## Changes from v1

| Rule | v1 | v2 | Reason |
|---|---|---|---|
| G2 | Edit only target `.cpp` files; no headers; read nothing else | Target is `.cpp` **plus included `.inl` files**; header readable; header editable only to add private helper declarations | Phase 1: 3 of 4 `.cpp` files contained only `#include`s of `.inl` files, so their code was out of scope. Header edits are needed for member helpers (G3). |
| G3 | Helpers are `static` free functions in an anonymous namespace | Helpers are **private member functions**, declared in the header, defined next to the source method; `const` only if the source method is `const`; never `UFUNCTION` | The v1 rule was the root cause of FT4 (callbacks to reach members: Sol 3, Grok 2) and FT7 (guessed member types: Grok 4). Member helpers access members directly. |
| G6 | Add only functions | Add only member functions; also no new member variables | Follows from G3. |
| W4 | ≤3 read-only parameters | ≤3 parameters **for local variables**; members accessed directly | Members no longer need to be passed in (G3). |
| W5 | All dependencies passed as plain parameters; no non-public member calls | **Removed** | Member helpers may call any member function. |
| N1 | Don't extract if a callback would be needed | Never use lambdas/callbacks/member pointers in helpers; call members directly | Reworded for member helpers; still targets FT4. |
| N2 | ≤3 params, no mutable member refs, no out-params | ≤3 params, no out-params | Members are accessed directly, so "mutable member refs" no longer applies. |
| N4 | Use template type parameter for unseen types, or don't extract | Copy types from visible declarations (method or header); otherwise don't extract | The header is now readable, so templates are no longer needed. Still targets FT7. |
| H2 | Pass what the fragment reads; follow N4 if types unseen | Pass only the **local variables** the fragment reads | Follows from G3 and N4. |
| H4 | Only change allowed: replace member access with a parameter | Only change allowed: replace locals with same-named parameters | Member access stays unchanged inside member helpers. |
| H7 | Re-read method and helper | Also re-read the header declaration | New edit location (header). |
| §5.2, §6.3–5, §7 | Static-helper wording | Member-helper wording; report adds "Header changes" | Follows from G2/G3. |

## Rule sources

Abbreviations:
- **Fowler** = M. Fowler, *Refactoring: Improving the Design of Existing Code*, 2nd ed. (2018). Chapter 1 defines refactoring and the "two hats", chapter 3 covers smells, and chapter 6 covers *Extract Function* and *Inline Function*. *Separate Query from Modifier* is in chapter 11.
- **T&C** = N. Tsantalis & A. Chatzigeorgiou, "Identification of Extract Method Refactoring Opportunities," CSMR 2009, and "Identification of extract method refactoring opportunities for the decomposition of methods," *JSS* 84(10), 2011.
- **Project** = a constraint set by the team.
- **Team threshold** = a number chosen by the team, motivated by a Fowler smell; Fowler gives no specific number.

Model columns: **C** = Claude Opus 5.5, **G** = Grok 4.7, **S** = GPT Sol 6.1. A number is how many times that model showed the failure in Phase 1. "—" means never.

| Rule | Rule summary | Source | Evidence / example | C | G | S |
|---|---|---|---|---|---|---|
| G1 | Goal is readability; behavior stays exactly the same | Fowler ch.1; FT2, FT9 | Grok iter 3 moved `GetActorLocation()` out of the per-actor loop | — | 1 | — |
| G2 | Target `.cpp` + included `.inl`; header only for adding private declarations | Project (v2 change) | 3 of 4 `.cpp` files were include-only. Possible cross-folder read by Grok (iter 1 matches Claude's helpers exactly; unconfirmed) | — | ? | — |
| G3 | Helpers are private member functions | Project (v2 change); FT4, FT7 | Sol's `[this](AActor* Player, const FVector& Location) { TryPlayerSightLead(Player, Location); }`; Grok's guessed `TArray<TWeakObjectPtr<AActor>>&` | — | 6 | 3 |
| G4 | Don't change Unreal macros | Project | All models complied | — | — | — |
| G5 | Only Extract Method changes; no renames, reformatting, comment moves, or bug fixes | Project; Fowler ch.2 "two hats"; FT8, FT11 | Grok moved the `// Squad identification shouts…` comment into a helper | — | 2 | — |
| G6 | Add only member functions; no new types or members | FT11 | Grok iter 2 added `struct FControllerSenseComponents` | — | 1 | — |
| G7 | "Unchanged" is a valid result | Project; FT12 | No model ever left `TrackerEnemy.cpp` unchanged on a re-run | 3 | 3 | 3 |
| W1 | Name the fragment by intent; extract if the name replaces a comment | Fowler ch.6 *Extract Function*; Fowler ch.3 *Comments* | Good example: `IsPlayerControlledPawn` (C, G) | — | — | — |
| W2 | One complete computation | T&C | Grok's `IsNoiseLeadSuppressed` computes a flag and the current time together | — | 1 | — |
| W3 | Single exit; guard extracted as `bool` predicate with `return` at call site | T&C (preconditions on jumps); FT8 | Good: `HasAnyValidPlayer` (C, G) / `HasVisiblePlayer` (S) | — | — | — |
| W4 | ≤3 parameters for locals, ≤1 return value | Fowler ch.3 *Long Parameter List* (team threshold); FT6 | Grok's `IsNoiseLeadSuppressed(...7 params..., double& OutNow)`; Sol's `HandlePlayerSightStimulus` has 5 params | — | 4 | 1 |
| W5 | *Removed in v2* | — | Superseded by G3 | — | — | — |
| W6 | Extract duplicated logic once; use at every identical site | Fowler ch.3 *Duplicated Code* | Good: `IsPlayerControlledPawn` and `GetSquadCoordinator` each replaced 2 copies (C, G) | — | — | — |
| N1 | No lambdas, callbacks, or member pointers in helpers | FT4 | Grok's `ShareIdentificationWithPingFallback(..., [this, Player]() { return PingTarget(...); })` | — | 2 | 3 |
| N2 | ≤3 params, no out-params | FT6; Fowler ch.3 *Long Parameter List* (team threshold) | Grok's `ClearTrackerPerceptionCaches` takes 5 member fields by reference | — | 4 | 1 |
| N3 | No helper-wrapping-helper, no pass-throughs, no single-call method bodies | FT5; Fowler ch.3 *Middle Man*, ch.6 *Inline Function* | Grok's `StoreControllerSenseComponents` wraps `FindControllerSenseComponents` | — | 6 | 2 |
| N4 | Don't guess types; copy from visible declarations | FT7 | Grok's `TArray<TWeakObjectPtr<AActor>>& VisiblePlayers` for a member it never saw | — | 4 | — |
| N5 | Leave short (~10 lines), single-purpose methods alone | Fowler ch.6 (team threshold); FT5 | Grok's 3-line `EndPlay` reduced to a call to `ReleaseTrackerFromSquad` | — | 1 | — |
| N6 | On re-runs, don't restructure existing helpers; report "no further opportunities" | FT12, FT5 | Grok iter 3 layered on iter-2 helpers; all models changed the file every iteration | 3 | 3 | 3 |
| H1 | Name by intent; `Is`/`Has` side-effect free; name covers everything; no shadowing | Fowler ch.6, ch.11 *Separate Query from Modifier*; FT10 | Grok's `IsNoiseLeadSuppressed` writes `OutNow`; Sol's parameter `TPingCallback PingTarget` shadows a member | — | 2 | 3 |
| H2 | Pass only locals the fragment reads; original types; `const` for read-only | Fowler ch.6 mechanics; FT7 | See N4 | — | 4 | — |
| H3 | Return at most one assigned local; otherwise don't extract | Fowler ch.6 mechanics; T&C | Grok's `struct FControllerSenseComponents` and `double& OutNow` | — | 2 | — |
| H4 | Copy the fragment verbatim | Fowler ch.6 mechanics; FT1, FT8 | Grok rewrote `const double Now = …` as `double Now = 0.0;`. Good: Sol's helpers are verbatim | — | 1 | — |
| H5 | Preserve evaluation order, call count, short-circuit order | Fowler ch.1; FT9 | Grok iter 3 calls `GetActorLocation()` once instead of once per actor | — | 1 | — |
| H6 | Leave call-site control flow and guards unchanged | FT8; T&C | Grok's `else if` → `return;`; Claude merged nested `if` in `EndPlay` | 1 | 4 | — |
| H7 | Replace whole statements; re-read method, helper, and header for truncation | FT1 | Claude: `const double Now` (iter 1), bare `return` in `PingTarget` (iter 3) | 2 | — | — |
| §5.1 | Never change behavior | Fowler ch.1; FT1, FT8, FT9 | See H5, H6, H7 | 3 | 5 | — |
| §5.5 | Never leave a statement truncated | FT1 | See H7 | 2 | — | — |
| §5.6 | Never guess unseen types | FT7 | See N4 | — | 4 | — |
| §5.7 | Don't claim compilation or behavior preservation without verifying | FT2 | Claude iter 2 blamed its own iter-1 error on the original | 3 | 1 | — |
| §6.1–2 | Self-check: every removed line reappears once; statements complete | FT1 | Catches both Claude truncations | 2 | — | — |
| §6.3–9 | Self-check: placement, parameters, types, order, control flow, scope, usefulness | Same sources as G3, W4, N1–N4, H4–H6 | — | — | — | — |
| §6.10 + §7 line column | Report line numbers from the file as it was at the start of the run | FT3 | Grok iter 2 cited `EndPlay` at 182–185 (actual 192–199); Claude iter 2 cited line 149 (actual 129) | 2 | 1 | — |
| §7 Header changes | Report every added declaration | Project (v2 change) | New in v2, so header edits can be checked against the diff | — | — | — |
| §7 Compilation field | Report "Built" or "Not built" explicitly | FT2 | Sol stated "Compilation could not be verified" (good). Claude iter 1 said "no errors" while broken | 1 | — | — |
| §7 Considered but not extracted | Record rejected candidates and the blocking rule | FT12; Project | Makes "no further opportunities" a reported outcome | — | — | — |

## Not yet verified

v2 has not been tested. Run it with Sol 6.1 on fresh copies of the 4 development classes (including their `.inl` files), then record the results and any new failure types here before writing v3.
