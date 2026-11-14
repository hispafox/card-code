<#
.SYNOPSIS
Comprueba que cada cabecera de C de una versión compila sola: un fichero con una
única línea, #include "<cabecera>", tiene que compilar sin nada más.
.EXAMPLE
.\scripts\comprobar-cabeceras.ps1 -Version despues
#>
param(
    [ValidateSet('antes', 'despues')]
    [string]$Version = 'despues'
)
. "$PSScriptRoot\comun.ps1"
Preparar-Gcc

$dirC = Join-Path (Carpeta-Version $Version) 'c'
$include = Join-Path $dirC 'include'
$salida = Nueva-Carpeta (Join-Path $Raiz "build\$Version\cabeceras")
$opciones = $OpcionesC + (Opciones-Version $dirC)

Write-Host "Cabeceras que compilan solas · $Version" -ForegroundColor Cyan
$fallan = 0
foreach ($cabecera in Get-ChildItem $include -Filter '*.h') {
    $prueba = Join-Path $salida ($cabecera.BaseName + '_sola.c')
    # La segunda línea evita el error de un fichero vacío cuando la cabecera solo trae macros.
    Set-Content -Path $prueba -Value @("#include `"$($cabecera.Name)`"", 'typedef int cabecera_sola;') -Encoding ascii
    # Los errores de gcc llegan por stderr; aquí no deben parar el script.
    $ErrorActionPreference = 'Continue'
    $mensajes = & $Gcc @($opciones + @('-I', $include, '-fsyntax-only', $prueba)) 2>&1 | ForEach-Object { "$_" }
    $codigo = $LASTEXITCODE
    $ErrorActionPreference = 'Stop'
    if ($codigo -eq 0) {
        Write-Host "  bien   $($cabecera.Name)"
    } else {
        $fallan++
        Write-Host "  falla  $($cabecera.Name)" -ForegroundColor Red
        $mensajes | Select-Object -First 3 | ForEach-Object { Write-Host "         $_" }
    }
}
if ($fallan -eq 0) {
    Write-Host 'Todas las cabeceras compilan solas.' -ForegroundColor Green
} else {
    Write-Host "Cabeceras que no compilan solas: $fallan." -ForegroundColor Red
    exit 1
}
