# Build and flash epd_sensor through ST-Link and OpenOCD.
# Usage: .\flash.ps1 [-SkipBuild] [-DryRun]

param(
    [switch]$SkipBuild,
    [switch]$DryRun
)

Set-StrictMode -Version Latest

if (-not $SkipBuild) {
    & (Join-Path $PSScriptRoot 'build.ps1')
}

$firmware = Join-Path $PSScriptRoot 'build/epd_sensor.hex'
if (-not (Test-Path -LiteralPath $firmware -PathType Leaf)) {
    throw "Firmware not found: $firmware. Run build.ps1 first."
}

$openocd = Get-Command openocd -ErrorAction SilentlyContinue
if ($null -eq $openocd) {
    throw 'openocd was not found in PATH.'
}

$firmwarePath = (Resolve-Path -LiteralPath $firmware).ProviderPath.Replace('\', '/')
$programCommand = "program {$firmwarePath} verify reset exit"
$arguments = @(
    '-f', 'interface/stlink.cfg',
    '-f', 'target/stm32l0.cfg',
    '-c', 'adapter speed 400',
    '-c', $programCommand
)

if ($DryRun) {
    Write-Host "OpenOCD: $($openocd.Source)"
    Write-Host "Command: $($arguments -join ' ')"
    return
}

& $openocd.Source @arguments
if ($LASTEXITCODE -ne 0) {
    throw "Flash or verification failed (exit code $LASTEXITCODE). Check target power, SWD wiring, VTref at VCC 1.8 V, and NRST when connecting from Stop. H2 pin 4 is VBAT, not VTref."
}

Write-Host 'Flash and verification complete.'
