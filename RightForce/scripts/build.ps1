[CmdletBinding()]
param(
    [string]$Compiler,
    [switch]$Run,
    [string]$Vice = 'x64sc'
)

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $Root 'build'

if (-not $Compiler) {
    if ($env:OSCAR64_HOME) {
        $Compiler = Join-Path $env:OSCAR64_HOME 'bin/oscar64.exe'
    } else {
        $Compiler = 'oscar64'
    }
}

if (Test-Path -LiteralPath $Compiler -PathType Leaf) {
    $CompilerPath = (Resolve-Path -LiteralPath $Compiler).Path
} else {
    $CompilerCommand = Get-Command $Compiler -ErrorAction SilentlyContinue
    if (-not $CompilerCommand) {
        throw "Oscar64 compiler not found. Pass -Compiler or set OSCAR64_HOME."
    }
    $CompilerPath = $CompilerCommand.Source
}

New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
Push-Location $Root
try {
    & $CompilerPath 'src/main.c' '-o=build/RightForce.prg'
    if ($LASTEXITCODE -ne 0) { throw 'Compilation failed' }
} finally {
    Pop-Location
}

Write-Host "Built: $BuildDir\RightForce.prg"
if ($Run) {
    & (Join-Path $PSScriptRoot 'run-vice.ps1') -Vice $Vice -Program (Join-Path $BuildDir 'RightForce.prg')
}
