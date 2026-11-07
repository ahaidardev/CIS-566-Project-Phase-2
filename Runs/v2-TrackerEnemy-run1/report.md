### TrackerEnemy.cpp
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| BindTrackerPerception | `void BindTrackerSuspicion()` | 50–54 | Finds the suspicion component and binds the suspicion-color-changed and target-identified events | 1 (BindTrackerPerception) |
| BindTrackerPerception | `void ReplayCurrentlySeenActors()` | 60–66 | Re-dispatches a sight stimulus for every actor the perception component already perceives | 1 (BindTrackerPerception) |
| HandleNoiseLead | `bool HasVisiblePlayer()` | 124–128 | Side-effect-free predicate: reports whether any tracked visible player is still valid (guard extracted per W3 exception; the original `return;` stays at the call site) | 1 (HandleNoiseLead) |

Header changes: added private declarations `void BindTrackerSuspicion();`, `void ReplayCurrentlySeenActors();`, `bool HasVisiblePlayer();` in the existing `private:` section of TrackerEnemy.h
Considered but not extracted: OnTrackerPerceptionUpdated sight branch (lines 76–86) — multiple mid-fragment `return`s targeting the enclosing function, blocked by W3; UnbindTrackerPerception reset block (lines 38–42) — method already does one coherent cleanup task, extraction adds no clarity (W1); OnTrackerPlayerIdentified guard (lines 138–140) — similar but not identical to the sight guard (extra `!GetWorld()`, `const APawn*`), and the method is already short (N5/W6); HandleNoiseLead cooldown check (lines 129–131) — `Now` is assigned to `LastNoisePingTime` after the check, splitting the computation across the boundary adds no clarity (W1/W2)
Behavior preservation: BindTrackerSuspicion — the five statements were moved verbatim, run in the same order at the same point (after the `GetController()` guard, before perception lookup), access the same members directly; ReplayCurrentlySeenActors — statements moved verbatim, called from the same position inside the `TrackerPerception.IsValid()` block after `AddUniqueDynamic`, so loop count, call order, and arguments are identical; HasVisiblePlayer — the guard loop is unchanged (`return;` became `return true;` inside the predicate, `return false;` falls through), and `if (HasVisiblePlayer()) return;` at the call site exits HandleNoiseLead exactly when the original loop did
Compilation: Not built (no Unreal Engine project/build files are present in this folder — only the two target class files, whose includes reference headers outside the allowed scope)
Pre-existing issues noticed (not changed): none

### TrackerEnemy.h
Status: Refactored

| Method | Helper (signature) | Lines moved (from file as it was at start of this run) | What it does | Call sites |
|---|---|---|---|---|
| — | — | — | No method bodies; edited only to declare the three new private helpers per G2/G3 | — |

Header changes: added `void BindTrackerSuspicion();`, `void ReplayCurrentlySeenActors();`, `bool HasVisiblePlayer();` to the existing `private:` section; nothing else changed
Considered but not extracted: none (header contains no method bodies)
Behavior preservation: declarations only; no executable code changed
Compilation: Not built (same reason as above)
Pre-existing issues noticed (not changed): none
