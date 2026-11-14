# Submódulo 2.1 · Nombres en un lenguaje sin espacios de nombres

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Prefijos · renombrar con el prefijo del módulo | `inicializar`, `verificar`, `extern uint8_t contador`; `iniciar`, `leer`, `escribir`; `datos` y `escrituras` globales | `pin_inicializar`, `pin_verificar`, `pin_intentos_restantes`; `memoria_inicializar`, `memoria_leer`, `memoria_escribir`; las variables, `static` | `c/include/pin.h`, `c/include/memoria.h` y sus `.c` |
| Prefijos · prefijar todo lo que la cabecera publica | `LONGITUD_MAX`, `EXITO`, `cabecera_t` | `COMANDO_LONGITUD_MAXIMA`, `COMANDO_SW_SIN_ERROR`, `comando_cabecera` | `c/include/comando.h` |
| Constantes · sustituir el número mágico | `validar` con `b[1] == 0x20`, `n < 5`, `b[4] > 8`, `0x6700`, `0x6983` | `comando_validar` con `COMANDO_POSICION_INS`, `COMANDO_LONGITUD_CABECERA`, `PIN_LONGITUD_MAXIMA`… | `c/src/comando.c` |
| Constantes · derivar una constante de las otras | `RESPUESTA_SALDO_LONGITUD 4u` con su comentario | `(SALDO_LONGITUD + COMANDO_SW_LONGITUD)` | `c/src/comando.c`, `c/include/saldo.h` |
| `enum` · agrupar las constantes relacionadas | Cuatro `#define COMANDO_INS_…` y un `switch` con `default` al que le falta la consulta | `typedef enum … comando_ins` y un `switch` sin `default`: el compilador avisa si falta un `case` | `c/include/comando.h`, `c/src/comando.c` |
| `static const` · dar tipo a la constante | `#define SALDO_MAXIMO 50000` | `static const uint16_t SALDO_MAXIMO = 50000u;` | `c/src/comando.c` |

Las pruebas (`c/pruebas/prueba_comando.c`) mandan comandos completos y miran el código de estado. Son las mismas en las dos versiones salvo dos cosas: las llamadas de preparación cambian de nombre (`iniciar` → `memoria_inicializar`, `personalizar` → `pin_personalizar`), y el después añade la prueba de la consulta del saldo, que en el antes responde «instrucción no admitida» porque el despacho no tiene su `case`.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
