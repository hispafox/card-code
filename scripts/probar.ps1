<#
.SYNOPSIS
Pasa todas las pruebas de una versión: las de C y, si la rama tiene applet, las de Java Card.
.EXAMPLE
.\scripts\probar.ps1 -Version antes
#>
param(
    [ValidateSet('antes', 'despues')]
    [string]$Version = 'despues'
)
. "$PSScriptRoot\comun.ps1"

$carpeta = Carpeta-Version $Version
$rojo = $false
if (Test-Path (Join-Path $carpeta 'c')) {
    & "$PSScriptRoot\probar-c.ps1" -Version $Version
    if ($LASTEXITCODE -ne 0) { $rojo = $true }
}
if (Test-Path (Join-Path $carpeta 'javacard')) {
    & "$PSScriptRoot\probar-java.ps1" -Version $Version
    if ($LASTEXITCODE -ne 0) { $rojo = $true }
}
if ($rojo) { exit 1 }
