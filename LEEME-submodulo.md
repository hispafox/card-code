# Submódulo 5.2 · Pruebas de caracterización

El módulo heredado `liquidacion`: una función, `liquidar`, que recorre los movimientos del día y devuelve el neto y la comisión. Es código antiguo, con los olores de 5.1, sin más prueba que una de cuando se escribió (`prueba_liquidacion.c`, «sin movimientos»), y compila sin `-Wconversion` (`c/compilar.opciones`), como el resto del código heredado del repositorio.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Fijar lo que hace hoy · una prueba por comportamiento, con el valor que da el código | Una prueba | Catorce pruebas nuevas en `prueba_caracterizacion.c` | `c/pruebas/prueba_caracterizacion.c` |
| Escribirla antes de tocar nada · la aserción que se sabe que falla | — | Los valores esperados salieron de ejecutar el código (abajo, la salida) | `c/pruebas/prueba_caracterizacion.c` |
| El comportamiento raro · fijarlo tal como está, con `RARO` delante | — | Tres: el pago de cero cobra 5, la devolución cobra comisión y solo el modo 1 redondea | `c/pruebas/prueba_caracterizacion.c` |

**El código no cambia.** `antes/c/src` y `despues/c/src`, y sus cabeceras, son idénticos; lo único nuevo del después son las pruebas.

**La aserción que se sabe que falla.** El primer borrador de dos pruebas esperaba comisión 0 para el pago de cero y para la devolución. La salida de `probar.ps1`, con la ruta recortada:

```
prueba_caracterizacion.c:60:test_un_pago_de_cero_cobra_cinco:FAIL: Expected 0 Was 5
prueba_caracterizacion.c:75:test_una_devolucion_suma_y_tambien_cobra:FAIL: Expected 0 Was 10
-----------------------
14 Tests 2 Failures 0 Ignored 
FAIL
```

El 5 y el 10 pasaron a la prueba, con su comentario `RARO`.

**Lo que las pruebas no fijan:** el contenido de `neto` y `comision` cuando `liquidar` devuelve `-1` o `-2` sí se fija (no se tocan); lo que pasa con un puntero nulo en `neto` o `comision`, no: `liquidar` no lo comprueba, y la prueba se caería en lugar de fallar.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
