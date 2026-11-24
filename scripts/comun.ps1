# Lo que comparten todos los scripts: dónde está el repositorio y dónde
# están las herramientas. Se carga con «. $PSScriptRoot\comun.ps1».
# No descarga nada: solo busca lo que ya está instalado.

$ErrorActionPreference = 'Stop'
# La salida de gcc, de Java y de las pruebas llega en UTF-8.
[Console]::OutputEncoding = [Text.Encoding]::UTF8
$Raiz = Split-Path $PSScriptRoot -Parent
$Lib = Join-Path $Raiz 'lib'
$Unity = Join-Path $Raiz 'terceros\unity'

# Opciones de compilación de C: C99 estricto, las conversiones que pueden
# cambiar un valor (-Wconversion, desde el submódulo 3.3), y cualquier aviso
# es un error. El código heredado que no las cumple las quita en su
# c/compilar.opciones con -Wno-conversion.
$OpcionesC = @('-std=c99', '-Wall', '-Wextra', '-Wpedantic', '-Wconversion', '-Werror')

function Buscar-Gcc {
    if ($env:CARDCODE_GCC -and (Test-Path $env:CARDCODE_GCC)) { return $env:CARDCODE_GCC }
    $enPath = Get-Command gcc -ErrorAction SilentlyContinue
    if ($enPath) { return $enPath.Source }
    $msys = 'C:\msys64\ucrt64\bin\gcc.exe'
    if (Test-Path $msys) { return $msys }
    throw 'No encuentro gcc. Instálalo (MSYS2, paquete mingw-w64-ucrt-x86_64-gcc) o indica su ruta en la variable CARDCODE_GCC.'
}

function Buscar-Java([string]$herramienta) {
    if ($env:JAVA_HOME) {
        $ruta = Join-Path $env:JAVA_HOME "bin\$herramienta.exe"
        if (Test-Path $ruta) { return $ruta }
    }
    $enPath = Get-Command $herramienta -ErrorAction SilentlyContinue
    if ($enPath) { return $enPath.Source }
    throw "No encuentro $herramienta. Instala un JDK 11 o posterior, o define JAVA_HOME."
}

function Preparar-Gcc {
    $script:Gcc = Buscar-Gcc
    $bin = Split-Path $script:Gcc -Parent
    # gcc de MSYS2 necesita su carpeta en el PATH para encontrar sus DLL.
    if (-not ($env:PATH -split ';' | Where-Object { $_ -eq $bin })) { $env:PATH = "$bin;$env:PATH" }
    $script:Size = Join-Path $bin 'size.exe'
}

# Ejecuta una herramienta y para el script si falla.
function Ejecutar([string]$exe, [string[]]$argumentos) {
    & $exe @argumentos | Out-Host
    if ($LASTEXITCODE -ne 0) { throw "Ha fallado: $(Split-Path $exe -Leaf) $($argumentos -join ' ')" }
}

function Carpeta-Version([string]$version) {
    $carpeta = Join-Path $Raiz $version
    if (-not (Test-Path $carpeta)) { throw "Esta rama no tiene la carpeta «$version»." }
    return $carpeta
}

function Nueva-Carpeta([string]$ruta) {
    if (Test-Path $ruta) { Remove-Item $ruta -Recurse -Force }
    New-Item -ItemType Directory -Path $ruta | Out-Null
    return $ruta
}

# Opciones de compilación propias de una versión, en c/compilar.opciones
# (por ejemplo, -DCRC_CON_ENSAMBLADOR para compilar la rutina en ensamblador).
function Opciones-Version([string]$dirC) {
    $fichero = Join-Path $dirC 'compilar.opciones'
    if (Test-Path $fichero) { return @((Get-Content $fichero -Raw).Trim() -split '\s+') }
    return @()
}

# Compila el código C de $dirC (include, src, pruebas) con Unity, un
# ejecutable por fichero de pruebas, y los ejecuta. Devuelve los ficheros en rojo.
function Probar-CodigoC([string]$dirC, [string]$salida) {
    Preparar-Gcc
    $opcionesVersion = Opciones-Version $dirC
    # Fuentes en C y en ensamblador (.S, que pasa por el preprocesador de C).
    $fuentes = @(Get-ChildItem (Join-Path $dirC 'src\*') -Include '*.c', '*.S' | ForEach-Object FullName)
    $pruebas = @(Get-ChildItem (Join-Path $dirC 'pruebas') -Filter 'prueba_*.c')
    if ($pruebas.Count -eq 0) { throw "No hay pruebas en $dirC\pruebas." }
    Nueva-Carpeta $salida | Out-Null
    $fallos = 0
    foreach ($prueba in $pruebas) {
        $exe = Join-Path $salida ($prueba.BaseName + '.exe')
        # Unity se compila aparte, sin -Werror: es código de terceros.
        $unityObj = Join-Path $salida 'unity.o'
        if (-not (Test-Path $unityObj)) { Ejecutar $Gcc @('-std=c99', '-c', (Join-Path $Unity 'unity.c'), '-o', $unityObj) }
        # Un fichero de pruebas puede pedir opciones propias en <nombre>.opciones
        # (por ejemplo, un espía de memset con -Wl,--wrap=memset).
        $extra = @()
        $ficheroOpciones = [IO.Path]::ChangeExtension($prueba.FullName, '.opciones')
        if (Test-Path $ficheroOpciones) { $extra = @((Get-Content $ficheroOpciones -Raw).Trim() -split '\s+') }
        Ejecutar $Gcc ($OpcionesC + $opcionesVersion + $extra + @('-I', (Join-Path $dirC 'include'), '-I', $Unity) +
                       $fuentes + @($prueba.FullName, $unityObj, '-o', $exe))
        & $exe | Out-Host
        if ($LASTEXITCODE -ne 0) { $fallos++ }
    }
    return [pscustomobject]@{ Ficheros = $pruebas.Count; Fallos = $fallos }
}

# Compila el applet de $dirJava\src contra jcardsim (que trae la API de
# Java Card 3.0.5), compila las especificaciones de Spock de $dirJava\pruebas
# y las ejecuta con el lanzador de JUnit. Devuelve $true si todo está en verde.
function Probar-JavaCard([string]$dirJava, [string]$salida) {
    $javac = Buscar-Java 'javac'
    $java = Buscar-Java 'java'
    $clases = Nueva-Carpeta (Join-Path $salida 'clases')
    $clasesPruebas = Nueva-Carpeta (Join-Path $salida 'clases-pruebas')
    $jcardsim = Join-Path $Lib 'jcardsim-3.0.6.0.jar'
    $asm = @('asm-7.2.jar', 'asm-commons-7.2.jar', 'asm-tree-7.2.jar', 'asm-util-7.2.jar', 'asm-analysis-7.2.jar') |
           ForEach-Object { Join-Path $Lib $_ }
    $groovy = Join-Path $Lib 'groovy-4.0.32.jar'
    $spock = @('spock-core-2.5-groovy-4.0.jar', 'hamcrest-3.0.jar', 'geantyref-1.3.16.jar') | ForEach-Object { Join-Path $Lib $_ }
    $junit = Join-Path $Lib 'junit-platform-console-standalone-1.14.4.jar'

    # El applet, con clases de Java 8: el conversor de Java Card no admite más.
    $java_fuentes = @(Get-ChildItem (Join-Path $dirJava 'src') -Recurse -Filter '*.java' | ForEach-Object FullName)
    Ejecutar $javac (@('--release', '8', '-encoding', 'UTF-8', '-nowarn', '-cp', $jcardsim, '-d', $clases) + $java_fuentes)

    # Las especificaciones de Spock, compiladas con el compilador de Groovy.
    $rutaCompilar = (@($groovy) + $spock + @($junit, $jcardsim, $clases)) -join ';'
    $especificaciones = @(Get-ChildItem (Join-Path $dirJava 'pruebas') -Recurse -Filter '*.groovy' | ForEach-Object FullName)
    Ejecutar $java (@('-cp', $rutaCompilar, 'org.codehaus.groovy.tools.FileSystemCompiler',
                      '-cp', $rutaCompilar, '-d', $clasesPruebas, '--encoding', 'UTF-8') + $especificaciones)

    $rutaEjecutar = (@($groovy) + $spock + @($jcardsim) + $asm + @($clases, $clasesPruebas)) -join ';'
    & $java '-Dfile.encoding=UTF-8' '-Dsun.stdout.encoding=UTF-8' -jar $junit execute --disable-banner --details=tree -cp $rutaEjecutar --scan-classpath $clasesPruebas --include-classname '.*Spec' --fail-if-no-tests | Out-Host
    return ($LASTEXITCODE -eq 0)
}
