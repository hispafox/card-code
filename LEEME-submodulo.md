# Submódulo 2.3 · La cabecera como contrato

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Cabecera y `.c` · llevar lo interno al `.c` | `comando.h` con tres `#include`, `COMANDO_REINTENTOS_LECTURA`, la estructura a la vista, `extern uint8_t comando_ultimo_ins` y `comando_copiar_datos` | Una constante, el tipo y las funciones documentadas; lo demás, `static` o `#define` dentro de `comando.c` | `c/include/comando.h`, `c/src/comando.c` |
| Cabecera y `.c` · hacer la cabecera autosuficiente | `comando.h`, `memoria.h`, `pin.h` y `registro.h` usan `uint8_t` sin incluir `<stdint.h>` | Cada cabecera incluye lo que usa, con su guarda, y `comando.c` incluye la suya la primera | `c/include/*.h` |
| Tipos opacos · esconder la estructura | `comando_respuesta` con sus tres campos en la cabecera | `typedef struct comando_respuesta comando_respuesta;` y la definición en `comando.c`, con `comando_respuesta_iniciar` | `c/include/comando.h`, `c/src/comando.c` |
| Tipos opacos · dar acceso con funciones | `registro.c` lee y escribe los campos, y comprueba contra 255 | `comando_respuesta_longitud`, `_llena` y `_anadir` | `c/src/registro.c` |
| Dependencias · declarar el tipo en lugar de incluir | `registro.h` incluye `comando.h` | `struct comando_respuesta;` | `c/include/registro.h` |
| Dependencias · sacar lo compartido a su cabecera | `pin.h` incluye `comando.h` por los códigos de estado, y `comando.h` incluye `pin.h` | `comando_sw.h`, que incluyen el PIN y el comando | `c/include/comando_sw.h`, `c/include/pin.h` |

**Una diferencia con la base, decidida por Pedro (14-11-2026):** en el antes, `comando.h` **lleva guarda**. La base dice que no la tiene, pero sin guarda y con el ciclo entre `pin.h` y `comando.h` el antes no compila: en cuanto un fichero incluye `comando.h`, `comando_respuesta` se define dos veces. Con la guarda, el antes compila y conserva el ciclo y las cabeceras que no incluyen `<stdint.h>`. Por eso los `.c` del antes incluyen `<stdint.h>` antes que nada.

**El script que lo enseña:** `.\scripts\comprobar-cabeceras.ps1` compila cada cabecera sola, con una única línea `#include`. En el antes fallan las cuatro (`uint8_t` sin definir); en el después compilan todas.

**Las pruebas:** `prueba_comando.c` tiene los mismos casos en las dos versiones; solo cambia cómo se prepara la respuesta y cómo se lee su longitud. El después trae además `prueba_registro.c`, que comprueba el cambio de comportamiento (con la respuesta llena, el registro lo anota y no escribe; en el antes, la comprobación contra 255 no saltaba nunca), y `prueba_pin.c`, que prueba el PIN sin el procesado de comandos.

**El tamaño:** mover declaraciones no cambia el binario, pero el tipo opaco sí: las funciones de acceso hacen crecer `comando.c` y `registro.c` (`medir-c.ps1`).

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\comprobar-cabeceras.ps1 -Version antes
.\scripts\comprobar-cabeceras.ps1 -Version despues
.\scripts\medir-c.ps1
```
