# Submódulo 5.1 · Los code smells más habituales

Un módulo nuevo, `extracto`, que escribe en la respuesta el extracto de los movimientos del registro: la versión del formato, el número de movimientos, una línea por movimiento (la letra del tipo y el importe), los totales por tipo si se piden y una suma de control. El antes trae los cinco olores del submódulo en una sola función; el después, una receta para cada uno.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Funciones largas · separar el bucle (*Split Loop*) | Un bucle escribe las líneas y suma los totales a la vez | Dos bucles: `extracto_escribir_movimientos` y `extracto_escribir_totales` | `c/src/extracto.c` |
| Funciones largas · extraer función (2.2) | `extracto_generar`, 74 líneas | Seis funciones `static` de 4 a 15 líneas; las dos públicas, de 8 y 12 (de la firma a la llave que cierra) | `c/src/extracto.c` |
| `switch` que crecen · sustituir el `switch` repetido por una tabla | Dos `switch` sobre el tipo, uno para la letra y otro para el total | `extracto_letras[]`, indexada por el tipo, y `totales[tipo]` | `c/src/extracto.c` |
| Duplicación · sustituir el código repetido por una llamada (*Replace Inline Code with Function Call*) | `>> 8` y `& 0xFF` escritos a mano cuatro veces | `util_escribir_u16`, que ya estaba en `utilidades.h` | `c/src/extracto.c` |
| Parámetros bandera · quitar el parámetro bandera (*Remove Flag Argument*) | `extracto_generar(…, int con_totales)` | `extracto_generar` y `extracto_generar_con_totales` | `c/include/extracto.h`, `c/src/extracto.c` |
| Comentarios · renombrar la variable (*Rename Variable*) | `t1`, `t2`, `t3` y `s`, cada una con su comentario | `totales[tipo]` y `suma` | `c/src/extracto.c` |
| Comentarios · el comentario hecho nombre | `/* comprobar que cabe: 2 de cabecera, 3 por movimiento… */`, `/* 1 pago, 2 recarga, 3 devolucion */` | `extracto_se_puede_generar`, las longitudes con nombre y `extracto_tipo` | `c/include/extracto.h`, `c/src/extracto.c` |

**Las pruebas:** `prueba_extracto.c`, nueve pruebas con los bytes exactos de cada extracto. Son los mismos casos en las dos versiones; cambia la llamada cuando se piden los totales (`…, 1)` en el antes, `extracto_generar_con_totales` en el después) y los nombres de los tipos, que el antes no tiene y la prueba declara para leerse.

**Un cambio que las pruebas no ven:** con un tipo desconocido, el antes ha escrito ya parte de la respuesta cuando devuelve 0; el después comprueba todos los tipos antes de escribir nada. Las dos devuelven 0, que es lo que mira quien llama.

**El tamaño:** el módulo pasa de 336 a 384 bytes con `-Os` (48 más).

**La duplicación que no se toca:** la comprobación doble del PIN de la rama de 3.5 (`comando_verificar_pin`) está repetida a propósito. Este módulo no tiene ninguna.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
