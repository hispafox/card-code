# Submódulo 2.2 · Funciones con una sola tarea

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Un nivel de abstracción · extraer función | `comando_pagar` junta los bytes del importe y del saldo a mano, con `apdu[5]` y `memoria_leer` | `comando_leer_importe` (`static`), `saldo_leer` y `saldo_escribir` (en `saldo.c`, nuevo) | `c/src/comando.c`, `c/src/saldo.c` |
| Retorno temprano · cláusulas de guarda | `comando_pagar` con cuatro niveles de `if` y sus `else` (cognitiva 14) | Cuatro guardas en el primer nivel y el camino normal al final (cognitiva 4) | `c/src/comando.c` |
| Retorno temprano · salida única con variable de resultado | `pin_comprobar_comando` sale por dos `return` antes del `memset`: la copia del PIN se queda en la pila | `resultado` y un solo borrado al final; las dos primeras guardas siguen saliendo antes, porque aún no hay nada que borrar | `c/src/pin.c` |
| Pocos parámetros · introducir un objeto parámetro | `comando_responder` con seis parámetros, el último un interruptor de cifrado | `comando_respuesta` y `comando_responder` con cuatro; el cifrado, en `comando_responder_cifrado` | `c/include/comando.h`, `c/src/comando.c` |
| Pocos parámetros · pasar la estructura entera | `registro_guardar_pago` con cuatro campos sueltos de `pago` | `registro_guardar_pago(&pago)` | `c/include/registro.h`, `c/src/registro.c` |
| `static` · hacer interna la función auxiliar | `construir_respuesta`, `copiar_datos` y `comprobar_longitud` visibles desde fuera; la última, sin usar | `comando_construir_respuesta` y `comando_copiar_datos`, `static`; `comprobar_longitud`, borrada | `c/src/comando.c` |
| `static` · esconder la variable del módulo | `uint8_t contador`, con su `extern` en la cabecera | `static uint8_t pin_intentos` y `pin_intentos_restantes()` | `c/src/pin.c`, `c/include/pin.h` |

Las pruebas de `c/pruebas/prueba_comando.c` son las mismas en las dos versiones, salvo las dos de la respuesta, que llaman a `comando_responder` con su firma de cada versión. El después trae además **la prueba nueva de la salida única**, `c/pruebas/prueba_pin_borrado.c`: comprueba que la copia del PIN queda borrada con el PIN correcto, con el equivocado y con un fallo de memoria. Como la copia es una variable local, la prueba espía `memcpy` y `memset` (el enlazador redirige sus llamadas; las opciones están en `prueba_pin_borrado.opciones`). Lanzada contra el código del antes, falla en las dos salidas tempranas.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
