# Submódulo 1.3 · La deuda técnica en un producto seguro

| Tema | Antes | Después | Dónde |
|---|---|---|---|
| La metáfora y su límite | `saldo_consultar` con un parámetro `con_historial` que hace de interruptor | Dos funciones, `saldo_consultar` e `historial_consultar`, que comparten `cabecera_preparar` | `c/src/consultas.c` |
| Por qué en un producto seguro sale más cara | `pin_verificar` gasta el intento después de comparar | El intento se gasta antes de comparar | `c/src/pin.c` |

Las pruebas de las consultas (`c/pruebas/prueba_consultas.c`) tienen los mismos casos y las mismas comprobaciones en las dos versiones; solo cambia la llamada, porque la función ya no lleva el parámetro. Las del PIN de `prueba_comandos.c` son idénticas y pasan en las dos: una prueba funcional normal no ve el orden en que se gasta el intento. El después trae además `prueba_pin.c`, la prueba nueva del cambio: simula un corte de alimentación a mitad de la comparación (con `memoria_simular_corte`) y comprueba que el intento se ha gastado aunque `pin_verificar` no llegue a devolver nada. Lanzada contra el código del antes, falla.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
