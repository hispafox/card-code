# Submódulo 3.2 · Memoria sin asignación dinámica

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Búferes estáticos · reservar el máximo al compilar | `comando_verificar_pin` copia el PIN a `pin_recibido[lc]`, un array de longitud variable cuyo tamaño decide el comando | `pin_recibido[PIN_LONGITUD_MAXIMA]` | `c/src/comando.c` |
| Búferes estáticos · un conjunto fijo en lugar de reservar al vuelo | Una sola respuesta, en un array estático | Dos respuestas reservadas al compilar, con `comando_respuesta_tomar` y `comando_respuesta_soltar`; `comando.h` gana el tipo opaco y las dos funciones | `c/src/comando.c`, `c/include/comando.h` |
| Tamaños con `sizeof` · medir el búfer donde se declara | `memset(pin_recibido, 0, lc)` en la verificación y `memset(pin_guardado, 0, 8)` en `pin_personalizar` | `sizeof` del propio array en los dos, y `UTIL_ELEMENTOS` en `utilidades.h` | `c/src/comando.c`, `c/src/pin.c`, `c/include/utilidades.h` |
| Tamaños con `sizeof` · pasar el tamaño con el puntero | `pin_copiar_guardado(destino)` copia ocho bytes escritos a mano | `pin_copiar_guardado(destino, n)`, llamada con `sizeof pin_guardado` | `c/src/pin.c` |
| Límites · validar la cabecera una vez, en la frontera | `comando_procesar` no compara `Lc` con los bytes que han llegado | La tercera condición: los bytes detrás de la cabecera son exactamente `Lc` | `c/src/comando.c` |
| Límites · comprobar la capacidad del destino antes de copiar | La verificación copia `lc` bytes sin mirar el tamaño del destino | `lc == 0u \|\| lc > sizeof pin_recibido` delante de la copia | `c/src/comando.c` |
| Punteros · llevar el final junto al puntero, y comparar con lo que queda | `comando_leer_personalizacion` avanza el puntero lo que dice cada campo | `comando_lector` con `actual` y `fin`, y `comando_leer_campo`, que compara con lo que queda antes de avanzar | `c/src/comando.c` |

**Lo que trae la rama para los dos:** el comando de personalización de fábrica (instrucción `50`, de la clase propietaria `80`), con dos campos, el PIN y el saldo inicial, cada uno precedido de su longitud. Solo se admite en una tarjeta sin PIN (`pin_personalizado()`). `pin_personalizar` relee lo que ha escrito para comprobarlo, en una copia local que borra al terminar. `PIN_LONGITUD_MAXIMA` se puede cambiar en una compilación de prueba.

**Las pruebas:** `prueba_comando.c` del después trae un PIN que dice traer 255 bytes, otro que dice traer 8 y solo trae la cabecera (los dos `67 00`, y el segundo sin gastar un intento), una personalización con un PIN de 200 bytes y otra sin el campo del saldo (`67 00` las dos) y el conjunto fijo de respuestas. En el antes no está la del PIN más largo que el máximo, porque el antes no lo rechaza. `prueba_pin_doce.c`, solo en el después, se compila con el PIN a doce bytes (`-DPIN_LONGITUD_MAXIMA=12u`, en su `.opciones`) y comprueba que los dos borrados limpian los doce; con el código del antes, la copia de comprobación llega a medias y la personalización falla.

**La práctica del bloque 3** junta esta rama y la de 3.1: leer el antes de las dos, lanzar sus scripts, leer el después y volver a lanzarlos.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
