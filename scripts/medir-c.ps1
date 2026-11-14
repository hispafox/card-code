<#
.SYNOPSIS
Mide cuántos bytes ocupa el código C de cada módulo, compilado para tamaño (-Os).
Sin -Version compara el antes con el después.
.EXAMPLE
.\scripts\medir-c.ps1
.\scripts\medir-c.ps1 -Version despues
#>
param(
    [ValidateSet('antes', 'despues', 'ambas')]
    [string]$Version = 'ambas'
)
. "$PSScriptRoot\comun.ps1"
Preparar-Gcc

# Compila cada fichero .c a objeto y devuelve, por módulo, el código y los
# datos constantes (text) más los datos inicializados (data), según «size».
function Medir([string]$version) {
    $dirC = Join-Path (Carpeta-Version $version) 'c'
    $salida = Nueva-Carpeta (Join-Path $Raiz "build\$version\objetos")
    $tabla = [ordered]@{}
    foreach ($fuente in Get-ChildItem (Join-Path $dirC 'src') -Filter '*.c') {
        $objeto = Join-Path $salida ($fuente.BaseName + '.o')
        Ejecutar $Gcc ($OpcionesC + @('-Os', '-I', (Join-Path $dirC 'include'), '-c', $fuente.FullName, '-o', $objeto))
        $linea = (& $Size $objeto | Select-Object -Skip 1 | Select-Object -First 1).Trim() -split '\s+'
        $tabla[$fuente.Name] = [int]$linea[0] + [int]$linea[1]
    }
    return $tabla
}

if ($Version -ne 'ambas') {
    $tabla = Medir $Version
    Write-Host "Tamaño del código C · $Version (bytes, -Os)" -ForegroundColor Cyan
    $tabla.GetEnumerator() | ForEach-Object { [pscustomobject]@{ Modulo = $_.Key; Bytes = $_.Value } } | Format-Table -AutoSize
    Write-Host "Total: $(($tabla.Values | Measure-Object -Sum).Sum) bytes"
    exit 0
}

$antes = Medir 'antes'
$despues = Medir 'despues'
$modulos = @($antes.Keys) + @($despues.Keys) | Sort-Object -Unique
Write-Host 'Tamaño del código C, antes y después (bytes, -Os)' -ForegroundColor Cyan
$filas = foreach ($m in $modulos) {
    $a = if ($antes.Contains($m)) { $antes[$m] } else { 0 }
    $d = if ($despues.Contains($m)) { $despues[$m] } else { 0 }
    [pscustomobject]@{ Modulo = $m; Antes = $a; Despues = $d; Diferencia = $d - $a }
}
$filas | Format-Table -AutoSize
$totalAntes = ($antes.Values | Measure-Object -Sum).Sum
$totalDespues = ($despues.Values | Measure-Object -Sum).Sum
Write-Host "Total: $totalAntes -> $totalDespues bytes (diferencia: $($totalDespues - $totalAntes))"
