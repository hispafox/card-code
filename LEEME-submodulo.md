# Submódulo 5.3 · Pruebas en Groovy

El applet es el del después de 4.5, sin cambios, en las dos versiones (`javacard/src/cardcode/tarjeta`). Lo que cambia son sus pruebas en Groovy, con Spock: el antes es una especificación escrita como se escribiría en JUnit, que pasa pero se lee mal; el después, las mismas comprobaciones contadas como comportamientos.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Pruebas legibles · el nombre dice el comportamiento | `test1` a `test4` | Frases: «no paga más de lo que hay, y el saldo no cambia» | `javacard/pruebas/*Spec.groovy` |
| Pruebas legibles · un comportamiento por prueba, con sus bloques | Cuatro métodos con todo dentro de un `setup:` y `assert` explícitos | `given:`, `when:`, `then:` y `expect:`; 18 pruebas | `ComandosSpec`, `PinSpec`, `SaldoSpec`, `RegistroSpec` |
| Pruebas legibles · una tabla de datos (`where:`) | Los tres fallos de PIN y los tres importes de recarga, uno detrás de otro | Dos tablas, que Spock ejecuta fila a fila con su nombre | `PinSpec`, `SaldoSpec` |
| Pruebas limpias · lo que repiten todas, a la clase base | Instalar, seleccionar y personalizar, copiado en los cuatro métodos; 30 `new CommandAPDU` | `TarjetaSimulada`: `setup()`, `verificar`, `pagar`, `recargar`, `saldo`, `llenarElRegistroConUnSaldoDe` | `javacard/pruebas/TarjetaSimulada.groovy` |
| Pruebas limpias · los códigos de estado con nombre | 21 códigos sueltos (`36864`, `0x6985`…) | `SW_CONDITIONS_NOT_SATISFIED` y los demás, con el nombre de la norma; ninguno suelto en las especificaciones | `javacard/pruebas/TarjetaSimulada.groovy` |

**Las mismas comprobaciones.** Las dos versiones comprueban lo mismo sobre el mismo applet: la personalización, el PIN (longitud, intentos y bloqueo), el saldo (sin PIN, recarga y pago, más de lo que hay, importes que no valen), el registro lleno y la instrucción desconocida. El antes las cuenta como 4 pruebas y el después como 18.

**El tamaño.** El antes ocupa 106 líneas en un fichero; el después, 198 en cinco (70 de la clase base).

**Cuando una prueba falla.** Con el applet de una copia aparte en la que el pago de más responde `6A80` en lugar de `6985`, la salida de las dos versiones (el árbol de JUnit, recortado):

```
      ├─ test2 ✘ Condition not satisfied:
      │     
      │        r.getSW() == 0x6985
      │        | |       |
      │        | 27264   false
      │        ResponseAPDU: 2 bytes, SW=6a80
```

```
      ├─ no paga más de lo que hay, y el saldo no cambia ✘ Condition not satisfied:
      │     
      │        respuesta.SW == SW_CONDITIONS_NOT_SATISFIED
      │        |         |  |  |
      │        |         |  |  27013
      │        |         |  false
      │        |         27264
      │        ResponseAPDU: 2 bytes, SW=6a80
```

En el antes, además, lo que `test2` comprobaba después de esa línea no llega a ejecutarse.

**`SimuladorConTransacciones`** es el de 4.5, en las dos versiones: lo necesitan las pruebas del registro lleno.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
