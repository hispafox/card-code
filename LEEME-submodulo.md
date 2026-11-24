# Submódulo 3.5 · Código redundante a propósito

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Comprobaciones duplicadas · un resultado que un fallo no pueda fabricar | `pin_verificar` devuelve 1 o 0: un bit de diferencia | `PIN_CORRECTO` (`0xA5C3`) y `PIN_INCORRECTO` (`0x5A3C`), complementarios | `c/include/pin.h`, `c/src/pin.c` |
| Comprobaciones duplicadas · comprobar dos veces la decisión crítica | `comando_verificar_pin` comprueba una vez | Dos comprobaciones; la segunda llama a `tarjeta_bloquear` | `c/src/comando.c` |
| Tiempo constante · comparar todos los bytes y acumular la diferencia | `pin_verificar` sale en el primer byte distinto, y antes si la longitud no coincide | `pin_diferencia` acumula la longitud y todos los bytes con `seguridad_diferencia` | `c/src/pin.c`, `c/src/seguridad.c` |
| Tiempo constante · decidir una sola vez, al final | — | Un único `if` sobre la diferencia, que convierte el resultado en una de las dos constantes | `c/src/pin.c` |
| Que el compilador no lo quite · pasar la decisión por una barrera | Sin barrera, `gcc -Os` junta las dos comprobaciones en una | `SEGURIDAD_BARRERA(resultado)` entre las dos | `c/include/seguridad.h`, `c/src/comando.c` |
| Que el compilador no lo quite · un borrado que el compilador no pueda quitar | `memset` de los búferes con un PIN | `seguridad_borrar`, que escribe a través de un puntero `volatile` | `c/src/seguridad.c` |
| Documentar · un comentario que diga qué protege y contra qué | — | Los comentarios `CONTRAMEDIDA (…)` | `c/src/comando.c`, `c/src/pin.c`, `c/src/seguridad.c`, `c/include/seguridad.h` |
| Documentar · las contramedidas, juntas en un módulo | — | `seguridad.h` y `seguridad.c` | `c/include/seguridad.h`, `c/src/seguridad.c` |

**El antes** es el después de 3.4 tal cual.

**El ensamblador:** `gcc -Os -S` sobre `despues/c/src/comando.c` enseña las dos comparaciones con `PIN_CORRECTO` en `comando_verificar_pin`; quitando la línea de la barrera, queda una. El de `despues/c/src/seguridad.c` enseña el borrado byte a byte.

**Las pruebas:** `prueba_seguridad.c`, solo en el después, sustituye `pin_verificar` con el enlazador (su `.opciones`) para que el comando reciba un resultado con unos bits cambiados, y comprueba que no da acceso. Comprueba también que con la tarjeta bloqueada el PIN correcto ya no sirve, que la comparación da distinto de cero si cambia el primer byte o el último y que un PIN de otra longitud es incorrecto. El salto de una instrucción provocado en el hardware no se puede simular en el portátil. Las pruebas del cambio de PIN y del PIN a doce bytes espían ahora `seguridad_borrar`, que es la que borra.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
