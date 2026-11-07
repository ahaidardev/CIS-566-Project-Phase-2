# Agentic Refactoring – Phase 2 to 4

Development and refinement of the Extract Method guide on the Wolf of the Desert codebase.
Phase 1 (model comparison) is archived in the previous repo.

```
CIS-566-Project-Phase-2/
├── Guide/
│   ├── refactor-guide-v2.md               current guide
│   ├── refactor-guide-v2-traceability.md  where each rule came from
│   └── scoring-checklist.md               template copied into every run
├── Development Set/WOTDRefactored/        untouched originals (4 classes, mirrors Source/WOTDRefactored)
├── Runs/                                  one isolated folder per run (see Runs/README.md)
└── new-run.ps1                            creates a fresh run folder for one class
```

Development set: TrackerEnemy, ABaseEnemy, AliCharacter, InventoryComponent
(4 `.cpp`, 4 headers, 18 `.inl` files).
