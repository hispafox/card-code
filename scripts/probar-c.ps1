<#
.SYNOPSIS
Compila el código C de una versión con sus pruebas (Unity) y las ejecuta.
.EXAMPLE
.\scripts\probar-c.ps1 -Version antes
#>
param(
    [ValidateSet('antes', 'despues')]
    [string]$Version = 'despues'
)
. "$PSScriptRoot\comun.ps1"

$dirC = Join-Path (Carpeta-Version $Version) 'c'
Write-Host "Pruebas de C · $Version" -ForegroundColor Cyan
$resultado = Probar-CodigoC $dirC (Join-Path $Raiz "build\$Version\pruebas-c")

if ($resultado.Fallos -eq 0) {
    Write-Host "Todas las pruebas de C de «$Version» en verde (ficheros de pruebas: $($resultado.Ficheros))." -ForegroundColor Green
} else {
    Write-Host "Hay pruebas de C en rojo en «$Version»: $($resultado.Fallos) de $($resultado.Ficheros) ficheros de pruebas." -ForegroundColor Red
    exit 1
}
