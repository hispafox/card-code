<#
.SYNOPSIS
Compila el applet de Java Card de una versión y pasa sus pruebas de Spock sobre jcardsim.
.EXAMPLE
.\scripts\probar-java.ps1 -Version despues
#>
param(
    [ValidateSet('antes', 'despues')]
    [string]$Version = 'despues'
)
. "$PSScriptRoot\comun.ps1"

$dirJava = Join-Path (Carpeta-Version $Version) 'javacard'
Write-Host "Pruebas del applet · $Version" -ForegroundColor Cyan
if (Probar-JavaCard $dirJava (Join-Path $Raiz "build\$Version\javacard")) {
    Write-Host "Todas las pruebas del applet de «$Version» en verde." -ForegroundColor Green
} else {
    Write-Host "Hay pruebas del applet en rojo en «$Version»." -ForegroundColor Red
    exit 1
}
