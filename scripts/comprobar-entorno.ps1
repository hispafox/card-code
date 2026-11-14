<#
.SYNOPSIS
Comprueba que el equipo tiene lo necesario para el repositorio: compila y
prueba un ejemplo mínimo en C (con Unity) y un applet mínimo (con jcardsim y Spock).
No descarga nada.
.EXAMPLE
.\scripts\comprobar-entorno.ps1
#>
. "$PSScriptRoot\comun.ps1"
$ejemplo = Join-Path $Raiz 'comprobacion-entorno'
$todoBien = $true

Write-Host 'Compilador de C' -ForegroundColor Cyan
Preparar-Gcc
Write-Host "  $Gcc"
& $Gcc --version | Select-Object -First 1
$c = Probar-CodigoC (Join-Path $ejemplo 'c') (Join-Path $Raiz 'build\comprobacion\c')
if ($c.Fallos -ne 0) { $todoBien = $false }

Write-Host 'JDK' -ForegroundColor Cyan
$java = Buscar-Java 'java'
Write-Host "  $java"
cmd /c "`"$java`" -version 2>&1" | Select-Object -First 1
if (-not (Probar-JavaCard (Join-Path $ejemplo 'javacard') (Join-Path $Raiz 'build\comprobacion\javacard'))) { $todoBien = $false }

if ($todoBien) {
    Write-Host 'El entorno está listo: C, Unity, el applet, jcardsim y Spock funcionan sin conexión.' -ForegroundColor Green
} else {
    Write-Host 'Algo del entorno falla: mira el primer error de arriba.' -ForegroundColor Red
    exit 1
}
