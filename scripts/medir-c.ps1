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

# Compila cada fichero .c a objeto y devuelve, por módulo, lo que acabaría en
# la memoria del chip: el código (.text), las constantes (.rdata) y los datos
# inicializados (.data). Se dejan fuera las tablas de excepciones de Windows
# (.pdata y .xdata) y la firma del compilador (.rdata$zzz), que en la tarjeta
# no existen, y la RAM sin inicializar (.bss).
function Medir([string]$version) {
    $dirC = Join-Path (Carpeta-Version $version) 'c'
    $salida = Nueva-Carpeta (Join-Path $Raiz "build\$version\objetos")
    $tabla = [ordered]@{}
    foreach ($fuente in Get-ChildItem (Join-Path $dirC 'src\*') -Include '*.c', '*.S') {
        $objeto = Join-Path $salida ($fuente.BaseName + '.o')
        Ejecutar $Gcc ($OpcionesC + (Opciones-Version $dirC) + @('-Os', '-I', (Join-Path $dirC 'include'), '-c', $fuente.FullName, '-o', $objeto))
        $bytes = 0
        foreach ($linea in & $Size -A $objeto) {
            $campos = "$linea".Trim() -split '\s+'
            if ($campos.Count -ge 2 -and $campos[0] -match '^\.(text|rdata|data)' -and $campos[0] -ne '.rdata$zzz') {
                $bytes += [int]$campos[1]
            }
        }
        $tabla[$fuente.Name] = $bytes
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
