# Submódulo 3.4 · Comportamiento indefinido y el optimizador

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Qué es · dar valor a cada variable al declararla | `registro_contar` deja `estado` sin valor cuando no hay ningún pago | `estado` nace con `MEMORIA_DATOS_CORROMPIDOS` | `c/src/registro.c` |
| Qué es · pasar las pruebas con el detector en modo trampa | — | `prueba_indefinido.c` se compila con `-fsanitize=undefined -fsanitize-undefined-trap-on-error` (su `.opciones`) | `c/pruebas/prueba_indefinido.c` |
| Casos habituales · desplazar en el tipo sin signo del resultado | `util_leer_u32` desplaza cada byte promovido a `int`: con el byte alto a `0x80` o más, es indefinido | Cada byte se convierte a `uint32_t` antes de desplazar | `c/include/utilidades.h` |
| Casos habituales · enteros sin signo para lo que cuenta | `static int32_t operaciones_totales` | `static uint32_t operaciones_totales` | `c/src/operaciones.c` |
| El optimizador · comprobar antes de operar | `sumar_cien` comprueba `total + 100 < total` | `total > INT32_MAX - 100` | `c/src/ejemplos.c` |
| El optimizador · comprobar el puntero antes de usarlo | `leer_primero` lee `*p` y después mira si `p` es nulo | El `if` va delante de la lectura | `c/src/ejemplos.c` |

**Lo que trae la rama para los dos:** la cuenta de pagos del registro (un byte y su CRC, que suma `registro_guardar_pago`) y el contador de operaciones, cuatro bytes de la memoria persistente que suman cada pago y cada recarga. `sumar_cien` y `leer_primero` son las dos funciones de demostración del capítulo, en `ejemplos.c`.

**El ensamblador del capítulo:** `gcc -Os -S` sobre `antes/c/src/ejemplos.c` y `despues/c/src/ejemplos.c`. En el antes, cada función son dos instrucciones; en el después aparecen las comparaciones.

**El aviso que apaga el antes:** `registro_contar` del antes da `-Wmaybe-uninitialized` cuando se compila con `-Os`, como hace `medir-c.ps1`, y no sin optimizar, como compilan las pruebas. Para que el script pueda medir el antes, `antes/c/compilar.opciones` apaga ese aviso.

**Las pruebas:** `prueba_contadores.c` está en las dos versiones. `prueba_indefinido.c`, solo en el después y en modo trampa, trae la cuenta sin pagos (`MEMORIA_DATOS_CORROMPIDOS`), el contador `0x80000001`, el contador que da la vuelta a cero y las dos funciones de demostración. Compilada con el código del antes, se para con una instrucción ilegal.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
