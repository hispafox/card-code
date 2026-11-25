# Submódulo 4.3 · Memoria persistente y transitoria

**El antes** es el después de 4.2 tal cual. Todo lo que el applet guarda en sus campos va a la memoria persistente, también lo que solo sirve mientras dura un comando o una sesión. **El después** pasa esos datos a memoria transitoria.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Los datos de trabajo · pasar el búfer de trabajo a un array transitorio | `recibido = new byte[8]`: el PIN que llega se copia ahí, en memoria persistente, y se queda después del comando | `JCSystem.makeTransientByteArray((short) 8, JCSystem.CLEAR_ON_DESELECT)` | `TarjetaApplet.java` |
| El estado de la sesión · la sesión, en memoria transitoria que se borra al deseleccionar | `boolean pinVerificado`, un campo: sigue a `true` después de sacar la tarjeta del lector | `boolean[] pinVerificado = JCSystem.makeTransientBooleanArray((short) 1, JCSystem.CLEAR_ON_DESELECT)`, y `pinVerificado[0]` donde se usa | `TarjetaApplet.java` |

Los dos arrays se crean en el constructor, como todo lo demás (4.1): lo transitorio es su contenido, el array es un objeto como otro cualquiera.

**Las pruebas:** `MemoriaSpec`, solo en el después, con cuatro pruebas que fallan con el código del antes. Verifica el PIN, reinicia la tarjeta (`tarjeta.reset()`), la vuelve a seleccionar e intenta una recarga: el antes la acepta (`9000`) y el después pide el PIN (`6982`). Lo mismo seleccionando otro applet entre medias (`OtroApplet`, en las pruebas). Comprueba con el simulador que `recibido` y `pinVerificado` son transitorios y que `pin` no lo es. Y que después de presentar un PIN incorrecto y seleccionar otro applet, `recibido` está a cero: en el antes sigue guardando los ocho bytes que llegaron.

**Lo que el simulador no enseña:** jcardsim no distingue la velocidad de escribir en una memoria o en otra, ni su desgaste, y sus contadores de memoria libre (`JCSystem.getAvailableMemory`) devuelven siempre 32 767. Lo que se comprueba aquí es dónde está cada dato y cuándo se borra.

**Lo que no es de este submódulo se queda como está:** el `switch` con la lógica de cada comando dentro y los números sueltos (4.4), y el pago que escribe el saldo antes de saber si cabe en el registro (4.5).

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
