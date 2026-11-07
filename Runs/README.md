# Runs

One folder per run, named `<guide version>-<class>-run<N>`, e.g. `v2-TrackerEnemy-run1`.

Create a folder with the script from the repo root (PowerShell):

```
.\new-run.ps1 -Class TrackerEnemy -GuideVersion v2 -Run 1
```

Each run folder contains:

```
v2-TrackerEnemy-run1/
├── refactor-guide-v2.md     the guide the model follows
├── PROMPT.md                paste this into the model
├── scoring.md               fill in after building and testing
├── report.md                written by the model
└── WOTDRefactored/          untouched copy of only this class's files
    ├── Public/...
    └── Private/...
```

## Rules

- Always start from a **fresh** run folder. Never run the model on top of an earlier run.
- Open the model **inside the run folder only**, so it can't see other runs or `Development Set/`.
- Commit twice per run: once before the model runs (original), once after (refactored). `git diff` between them is the evidence.
- Never edit `Development Set/`. It is the untouched baseline.
- The 6 evaluation classes do not go in this repo until the final evaluation.

## Building and testing a run

1. In a clean copy of the full Unreal project, copy the run's `WOTDRefactored/` over `Source/WOTDRefactored/`.
2. Build the project.
3. Run the Automation tests that cover the class, plus your own test cases.
4. Fill in `scoring.md`.
5. Reset the full project: `git checkout .`
