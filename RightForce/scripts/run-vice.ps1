[CmdletBinding()]
param(
    [string]$Vice = 'x64sc',
    [string]$Program
)

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
if (-not $Program) {
    $Program = Join-Path $Root 'build/RightForce.prg'
}
if (-not (Test-Path -LiteralPath $Program -PathType Leaf)) {
    throw "Program not found: $Program. Run scripts/build.ps1 first."
}

if (Test-Path -LiteralPath $Vice -PathType Leaf) {
    $VicePath = (Resolve-Path -LiteralPath $Vice).Path
} else {
    $ViceCommand = Get-Command $Vice -ErrorAction SilentlyContinue
    if (-not $ViceCommand) {
        throw "VICE x64sc not found. Pass -Vice with the full path to x64sc."
    }
    $VicePath = $ViceCommand.Source
}

& $VicePath -pal -autostart (Resolve-Path -LiteralPath $Program).Path
