# Submódulo 5.4 · Refactorizar en pasos pequeños

La práctica del bloque 5. El antes es el módulo heredado `liquidacion` con sus pruebas de caracterización, tal como queda en el después de 5.2. El después es el mismo módulo al final de una serie de pasos, **uno por commit**, cada uno con las pruebas pasadas y el tamaño medido antes de seguir. Para ver un paso: `git show <commit>`; para probarlo o medirlo, `git checkout <commit>` y los scripts de siempre (y `git checkout submodulo-5.4/pasos-pequenos` para volver).

| Commit | Paso | Pruebas | `liquidacion.c` (bytes, `-Os`) |
|---|---|---|---|
| `d984287` | Punto de partida: el después de 5.2 | 15 en verde | 176 |
| `8070454` | 1 · La comisión, repetida en el pago y en la devolución, a una función | 15 en verde | 176 |
| `70efa91` | 2 · Los tipos, los códigos de error y las constantes de la comisión, con nombre | 15 en verde | 176 |
| `2820d0c` | 3 · Nombres que dicen lo que guardan | 15 en verde | 176 |
| — | Intento del paso 4, **sin commit**: juntar la recarga y la devolución. En rojo, y deshecho con `git restore` | 2 en rojo | 176 |
| `ec73284` | 4 · El redondeo de la comisión, a su función | 15 en verde | 176 |
| `8fccee6` | 5 · El recorrido de los movimientos, separado de la entrega del resultado | 15 en verde | 176 |
| `2add47d` | 6 · `liquidacion_calcular` y `liquidacion_calcular_redondeando`, una por cada valor del modo; `liquidar` las llama | 15 en verde | 272 |
| `4842d32` | 7 · Las pruebas llaman a las funciones nuevas; la del modo 2 se va | 14 en verde | 272 |
| `fbeaed8` | 8 · Fuera `liquidar` | 14 en verde | 256 |

Las pruebas son las de `prueba_caracterizacion.c` (14, hasta el paso 7, que deja 13) más la antigua de `prueba_liquidacion.c`.

**El intento que salió en rojo.** Después del paso 3 se intentó juntar los dos `case` que suman el importe al neto, la recarga y la devolución. No son iguales: la devolución cobra comisión, el comportamiento raro que 5.2 fijó tal como estaba. El cambio, sobre `despues/c/src/liquidacion.c` del paso 3:

```
         case LIQUIDACION_RECARGA:
-            acumulado = acumulado + movimientos[i].imp;
-            break;
         case LIQUIDACION_DEVOLUCION:
             acumulado = acumulado + movimientos[i].imp;
-            comision_total = comision_total + liquidacion_comision(movimientos[i].imp);
             break;
```

La salida de `probar.ps1`, con la ruta recortada (es la del fichero de caracterización; la prueba antigua siguió en verde):

```
prueba_caracterizacion.c:75:test_una_devolucion_suma_y_tambien_cobra:FAIL: Expected 10 Was 0
prueba_caracterizacion.c:84:test_varios_movimientos_se_acumulan:FAIL: Expected 20 Was 15
-----------------------
14 Tests 2 Failures 0 Ignored 
FAIL
```

No llegó a tener commit: se deshizo con `git restore despues`, que deja la carpeta como en el último commit en verde, y el paso 4 fue otro. Si un paso ya tuviera commit, se volvería con `git revert`, que deja constancia en el historial.

**El tamaño.** De los pasos 1 a 5, el compilador saca exactamente las mismas instrucciones que en el punto de partida (comparado con `objdump -d`; con `gcc -Os -S`, iguales salvo los números de tres etiquetas internas en el paso 4): los nombres, las constantes y estas funciones `static` no cuestan nada. El paso 6 añade dos funciones públicas, y las dos llaman al recorrido; al quitar `liquidar` en el paso 8 quedan 256 bytes, 80 más que al empezar. `medir-c.ps1` suma secciones, que este compilador alinea a 16 bytes: el intento en rojo ocupaba 162 bytes de código y la medida decía 176, igual que los pasos en verde.

**Lo que queda para otro día:** `struct mov` con sus campos `t` e `imp`, `MAXMOV`, la variable global `ult_comision` y el `-Wno-conversion` de `c/compilar.opciones`. Cambiarlos toca a quien llama y no hace falta para lo que se quería.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
