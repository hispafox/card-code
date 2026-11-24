# Submódulo 1.4 · Los principios que valen en cualquier lenguaje

| Tema | Antes | Después | Dónde |
|---|---|---|---|
| Una sola responsabilidad | `proc` decide qué comando ha llegado, procesa los tres y prepara la respuesta, con veinte variables globales | `comando_despachar` decide; `comando_verificar_pin`, `comando_pagar` y `comando_recargar` procesan cada uno lo suyo | `c/src/comandos.c` |
| Depender poco de los demás | El pago y la recarga leen el saldo en `0x0040` y `0x0041` | El módulo `saldo` es el único que sabe dónde está el saldo; el resto llama a `saldo_leer` y `saldo_escribir` | `c/src/saldo.c` |
| Nombres que explican | `chk`, `b`, `n`, `tmp`, `flag2`, `buf2` | `pin_verificar`, `apdu`, `longitud`, `importe`, `bufer_datos` | `c/src/comandos.c`, `c/src/pin.c` |

**Práctica del bloque 1:** leer en grupo `antes/c/src/comandos.c` (y `antes/c/src/pin.c`, que toca una de sus variables globales) con los seis pasos del capítulo.

Las pruebas (`c/pruebas/prueba_comandos.c`) son idénticas en las dos versiones: solo mandan comandos y miran la respuesta, sin saber cómo está hecho el módulo por dentro.

**El aviso que apaga el antes:** desde el submódulo 3.3, las opciones del repositorio llevan `-Wconversion`. El antes es el código heredado del curso y da cuatro avisos con esa opción (tres de `-Wsign-conversion`, por un `int` que se pasa como tamaño), así que `antes/c/compilar.opciones` la quita con `-Wno-conversion`. El después compila con ella.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
