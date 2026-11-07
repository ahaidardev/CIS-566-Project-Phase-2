<#
.SYNOPSIS
  Creates a fresh, isolated run folder for one class.

.EXAMPLE
  .\new-run.ps1 -Class TrackerEnemy -GuideVersion v2 -Run 1
  .\new-run.ps1 -Class AliCharacter -GuideVersion v2 -Run 1 -Focus 11_PowerWeaponAndViewMode.inl

  Creates Runs\v2-<Class>-run<N>\ containing only that class's untouched files
  (copied from "Development Set"), the guide, PROMPT.md, and an empty scoring.md.
  Open the model in that folder only and paste PROMPT.md.
#>
param(
    [Parameter(Mandatory)]
    [ValidateSet('TrackerEnemy', 'ABaseEnemy', 'AliCharacter', 'InventoryComponent')]
    [string]$Class,

    [Parameter(Mandatory)]
    [string]$GuideVersion,          # e.g. v2

    [int]$Run = 1,

    # Optional: limit the refactoring to specific .inl files (large classes like AliCharacter).
    [string[]]$Focus = @()
)

$ErrorActionPreference = 'Stop'
$root  = $PSScriptRoot
$src   = Join-Path $root 'Development Set\WOTDRefactored'
$guide = Join-Path $root "Guide\refactor-guide-$GuideVersion.md"
$check = Join-Path $root 'Guide\scoring-checklist.md'
$name  = "$GuideVersion-$Class-run$Run"
$dest  = Join-Path $root "Runs\$name"

if (Test-Path $dest)       { throw "$dest already exists. Use a new -Run number." }
if (-not (Test-Path $guide)) { throw "Guide not found: $guide" }

# Files for each class, relative to WOTDRefactored\
$map = @{
    TrackerEnemy       = @('Public\Enemy\TrackerEnemy.h',
                           'Private\Enemy\TrackerEnemy.cpp')
    ABaseEnemy         = @('Public\Enemy\ABaseEnemy.h',
                           'Private\Enemy\ABaseEnemy.cpp',
                           'Private\Enemy\ABaseEnemy')
    AliCharacter       = @('Public\Player\AliCharacter.h',
                           'Private\Player\AliCharacter.cpp',
                           'Private\Player\AliCharacter')
    InventoryComponent = @('Public\Player\Components\InventoryComponent.h',
                           'Private\Player\Components\InventoryComponent.cpp',
                           'Private\Player\Components\InventoryComponent')
}

# Copy the class's untouched files, keeping the same paths as the real project.
foreach ($rel in $map[$Class]) {
    $from = Join-Path $src $rel
    $to   = Join-Path $dest "WOTDRefactored\$rel"
    New-Item -ItemType Directory -Force -Path (Split-Path $to) | Out-Null
    Copy-Item -Path $from -Destination $to -Recurse
}

Copy-Item $guide (Join-Path $dest "refactor-guide-$GuideVersion.md")
if (Test-Path $check) { Copy-Item $check (Join-Path $dest 'scoring.md') }

# List the copied files for the prompt.
$fileList = Get-ChildItem -Path (Join-Path $dest 'WOTDRefactored') -Recurse -File |
    ForEach-Object { '- ' + $_.FullName.Substring($dest.Length + 1).Replace('\', '/') }

$focusText = ''
if ($Focus.Count -gt 0) {
    $focusText = "`nOnly refactor methods in these files: $($Focus -join ', '). Leave all other files unchanged.`n"
}

$prompt = @"
Follow refactor-guide-$GuideVersion.md in this folder exactly.

Target class: $Class. Its files are:
$($fileList -join "`n")
$focusText
Apply Extract Method refactoring to this class as the guide allows. Do not read or edit anything outside this folder.

When finished, write your report, in the format from section 7 of the guide, to report.md in this folder.
"@

Set-Content -Path (Join-Path $dest 'PROMPT.md') -Value $prompt -Encoding UTF8

Write-Host "Created $dest"
Write-Host "Next:"
Write-Host "  1. git add `"Runs\$name`" ; git commit -m `"${name}: original`""
Write-Host "  2. Open the model in that folder only and paste PROMPT.md"
Write-Host "  3. git add `"Runs\$name`" ; git commit -m `"${name}: refactored`""
Write-Host "  4. Copy WOTDRefactored\ over the full project's Source\WOTDRefactored\, build, run tests, fill scoring.md"
