# Submódulo 2.4 · Macros, `static inline` y el ensamblador aislado

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Macros · sustituir la macro con lógica por `static inline` | `MINIMO`, `LEER_U16` y `ESCRIBIR_U16` | `util_minimo`, `util_leer_u16` y `util_escribir_u16` | `c/include/utilidades.h`; se usan en `comando.c`, `saldo.c` y `registro.c` |
| Macros · hacer segura la macro que se queda | `COMPROBAR(c)`, que dentro de un `if` con `else` cambia de sentido (`pin_cambiar`) | `UTIL_COMPROBAR(c)` con `do { … } while (0)` y `util_fallo(__FILE__, __LINE__)` | `c/include/utilidades.h`, `c/src/pin.c` |
| Medir el binario | — | `.\scripts\medir-c.ps1` compara módulo a módulo | `scripts/medir-c.ps1` |
| Ensamblador · interfaz en C delante | `crc16_x64` declarada a mano con `extern` en `comando.c`, `memoria.c` y `registro.c` | `crc.h` con `crc_calcular`; los tres módulos la incluyen | `c/include/crc.h` |
| Ensamblador · versión en C al lado | `crc16_x64.S` sin compañía | `crc.c` elige con `#if defined(CRC_CON_ENSAMBLADOR)` entre `crc_x64.S` y `crc_calcular_en_c` | `c/src/crc.c`, `c/src/crc_x64.S` |

**La rutina en ensamblador** es para el procesador del portátil (x86-64, sintaxis de GNU as) y vale para Windows y Linux. El después la compila porque `despues/c/compilar.opciones` define `CRC_CON_ENSAMBLADOR`. Las pruebas del CRC pasan dos veces: `prueba_crc.c` con el ensamblador y `prueba_crc_en_c.c` con la versión en C (su `.opciones` quita la definición). Dan los mismos valores.

**El aviso que apaga el antes:** gcc con `-Wall` avisa del `COMPROBAR` dentro de un `if` con `else` (`-Wdangling-else`, «suggest explicit braces to avoid ambiguous else»), y con las opciones del repositorio, que convierten los avisos en errores, el antes no compilaría. Para poder enseñar el fallo, `antes/c/compilar.opciones` apaga ese aviso. Es un dato útil para la clase: con gcc y `-Wall`, este error concreto lo encuentra el compilador.

**Las pruebas:** `prueba_comando.c` es la misma en las dos versiones. El después trae además `prueba_utilidades.c`, con los tres usos que rompían las macros (`*p++` con el mínimo, un `if` sin llaves al escribir dos bytes y la comprobación dentro de un `if` con `else`); en el antes no están, porque fallarían.

**Práctica del bloque 2:** leer `antes/c/include/utilidades.h` y sus usos, lanzar el script del antes, leer el después y lanzar `medir-c.ps1`.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
