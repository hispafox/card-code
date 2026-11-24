# Submódulo 3.1 · Errores que no se pierden

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Códigos de retorno · comprobar el resultado y pasarlo hacia arriba | `comando_pagar` llama a `saldo_escribir` y `registro_guardar_pago` sin mirar lo que devuelven, y `comando_recargar` hace lo mismo con su `saldo_escribir`: tres resultados descartados | Cada resultado se compara con `MEMORIA_SIN_ERROR`, y si falla el comando responde `65 81` (`COMANDO_SW_FALLO_MEMORIA`) | `c/src/comando.c` |
| Códigos de retorno · marcar el resultado como obligatorio | Sin marca: la llamada sin comprobar compila | `UTIL_NO_IGNORAR` (`warn_unused_result` de GCC) en `saldo_escribir`, `registro_guardar_pago` y `saldo_leer` | `c/include/utilidades.h`, `c/include/saldo.h`, `c/include/registro.h` |
| Un único punto de limpieza · salir hacia un único punto de limpieza | `comando_cambiar_pin` repite la limpieza en cada salida, y la del fallo al escribir el PIN nuevo no cierra el modo de personalización | Las salidas con algo que deshacer saltan con `goto limpiar` a una sola etiqueta | `c/src/comando.c` |
| Un único punto de limpieza · empezar por el fallo | — | `resultado` empieza valiendo `COMANDO_SW_SIN_DIAGNOSTICO` (`6F 00`) | `c/src/comando.c` |
| Errores hasta quien los trata · separar el valor del estado | `saldo_leer` devuelve 0 si el saldo no cuadra con su CRC, que es también el saldo de una tarjeta vacía; `registro_leer_pago` devuelve 1 o 0 | `saldo_leer(uint16_t *saldo)` y `registro_leer_pago` devuelven un `memoria_estado` y solo escriben el valor si ha ido bien | `c/src/saldo.c`, `c/src/registro.c` |
| Errores hasta quien los trata · traducir el error en la frontera | Cada comando traduce a mano | `comando_sw_de`, un `switch` sin `default` | `c/src/comando.c` |

**Lo que trae la rama para los dos:** la memoria persistente dice si una escritura ha ido bien (`memoria_estado` en `memoria.h`), y las pruebas pueden pedirle que falle la siguiente con `memoria_fallar_siguiente_escritura()`. El saldo se guarda con su CRC, como el registro. El cambio de PIN es la instrucción `24` (CHANGE REFERENCE DATA en ISO/IEC 7816-4) y trae el PIN actual y el nuevo, ocho bytes cada uno.

**Las pruebas:** `prueba_comando.c` del después trae dos pruebas nuevas, el pago con la escritura del saldo fallando (`65 81`; el antes respondería `90 00`) y el saldo corrompido, con el que el pago y la recarga responden `65 81` sin escribir nada (el antes recargaría y taparía la corrupción con un CRC bueno). `prueba_cambio_pin.c`, solo en el después, espía `memcpy` y `memset` con el enlazador (su `.opciones`) y comprueba que el modo se cierra y los dos búferes se borran salga por donde salga el comando; en el antes, con la escritura fallando, el modo se queda abierto.

**La huella de la memoria:** la prueba de la huella escribe un byte suelto. El saldo y el registro llevan su CRC detrás, y un bloque que termina en su propio CRC no cambia el CRC de toda la memoria.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
