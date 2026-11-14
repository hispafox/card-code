# card-code

Repositorio de ejemplo del curso de código limpio en C y Java Card. El dominio es una tarjeta con saldo, recarga, pago y PIN: la parte en C recibe los comandos, verifica el PIN y escribe en la memoria persistente (simulada en el equipo de desarrollo), y el applet de Java Card lleva el saldo.

Todo el código está escrito y probado. Los scripts solo compilan, pasan las pruebas y miden; ninguno crea ni modifica código.

## Ramas

- `main`: lo común a todas las ramas (scripts, librerías, Unity) y un ejemplo mínimo para comprobar el entorno.
- `submodulo-M.S/<tema>`: una por submódulo del curso. Cada una trae dos carpetas, las dos compilando y con sus pruebas en verde:
  - `antes/`: el código con el problema.
  - `despues/`: el código arreglado.

Dentro de cada versión, `c/` lleva el código en C (`include/`, `src/` y `pruebas/`) y, cuando la rama tiene applet, `javacard/` lleva el applet (`src/`) y sus pruebas de Spock (`pruebas/`).

## Qué hace falta

- **gcc** (en Windows, el de MSYS2). Los scripts lo buscan en la variable `CARDCODE_GCC`, después en el `PATH` y por último en `C:\msys64\ucrt64\bin`.
- **Un JDK 11 o posterior**, en `JAVA_HOME` o en el `PATH`.
- Nada más: Groovy, Spock, jcardsim y el lanzador de JUnit van en `lib/`, y Unity en `terceros/unity/`.

## Los scripts

Desde la raíz del repositorio, en PowerShell:

| Script | Qué hace |
|---|---|
| `.\scripts\comprobar-entorno.ps1` | Compila y prueba el ejemplo mínimo en C y en Java Card. Si sale en verde, el equipo está listo |
| `.\scripts\probar.ps1 -Version antes` | Pasa todas las pruebas de una versión (`antes` o `despues`) |
| `.\scripts\probar-c.ps1 -Version despues` | Solo las pruebas de C |
| `.\scripts\probar-java.ps1 -Version despues` | Solo las del applet, con jcardsim y Spock |
| `.\scripts\comprobar-cabeceras.ps1 -Version despues` | Comprueba que cada cabecera de C compila sola, con una única línea `#include` |
| `.\scripts\medir-c.ps1` | Compila cada módulo de C para tamaño (`-Os`) y compara los bytes del antes y del después |

Si PowerShell no deja ejecutar scripts, ábrelo con `powershell -ExecutionPolicy Bypass` o lanza cada uno con `powershell -ExecutionPolicy Bypass -File .\scripts\<script>.ps1`.

## Sin conexión

Nada se descarga al compilar ni al probar. Las librerías de `lib/` están dentro del repositorio con sus licencias (`lib/LEEME.md`).
