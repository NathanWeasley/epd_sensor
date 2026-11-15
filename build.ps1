# Build epd_sensor with the project's Makefile.
# Usage: .\build.ps1

Set-StrictMode -Version Latest

$make = Get-Command make -ErrorAction SilentlyContinue
if ($null -eq $make) {
    throw 'make was not found in PATH.'
}

& $make.Source -C $PSScriptRoot --no-print-directory -j2 all
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE."
}

foreach ($extension in @('elf', 'hex', 'bin')) {
    $artifact = Join-Path $PSScriptRoot "build/epd_sensor.$extension"
    if (-not (Test-Path -LiteralPath $artifact -PathType Leaf)) {
        throw "Build completed without expected artifact: $artifact"
    }
}

Write-Host "Build complete: $(Join-Path $PSScriptRoot 'build/epd_sensor.hex')"
