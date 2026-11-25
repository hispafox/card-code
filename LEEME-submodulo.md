# Submódulo 4.5 · Excepciones y transacciones

**El antes** es el después de 4.4 con lo nuevo de este capítulo: alguien ha protegido el cambio de PIN con una transacción escrita como en Java, con un `try`, un `catch (Exception e)` que la aborta y responde `6F00`. El pago y la recarga siguen sin transacción. **El después** deja que las excepciones lleguen al entorno con su código, pone la transacción donde hacen falta dos escrituras juntas y la quita de donde hacía daño.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Los errores, con su código · dejar que la `ISOException` llegue al entorno | `catch (Exception e)` en el cambio de PIN: un PIN incorrecto o bloqueado sale como `6F00` | Sin `catch`: cada `ISOException` llega al entorno de ejecución con su código | `TarjetaApplet.java`, `cambiarPin` |
| Los errores, con su código · un código para lo que el terminal tiene que hacer distinto | Un PIN incorrecto responde siempre `6982` | `63CX`, con X los intentos que quedan (`SW_VERIFICACION_FALLIDA \| intentos`) | `TarjetaApplet.java`, `comprobarPin` |
| Escrituras que no se quedan a medias · una transacción alrededor de las escrituras que van juntas | El pago y la recarga escriben el saldo y después anotan en el registro; si el registro está lleno, el saldo ya ha cambiado | `JCSystem.beginTransaction()` antes de la primera escritura y `commitTransaction()` después de la última | `TarjetaApplet.java`, `pagar` y `recargar` |
| Qué entra en una transacción · el contador del PIN, fuera | `comprobarPin` dentro de la transacción del cambio de PIN: al abortarla, el intento gastado vuelve | `comprobarPin` antes de escribir nada, sin transacción | `TarjetaApplet.java`, `cambiarPin` |
| Qué entra en una transacción · lo que ya es atómico, sin transacción | La copia del PIN nuevo dentro de una transacción | `Util.arrayCopy`, que copia el array entero o lo deja como estaba | `TarjetaApplet.java`, `cambiarPin` |

**El simulador con transacciones.** jcardsim lleva la cuenta de si hay una transacción abierta, pero al abortarla no deshace nada, y si un comando termina con una excepción no la aborta: la transacción se queda abierta y el siguiente `beginTransaction` falla. La especificación del entorno de ejecución dice lo contrario (JCRE 3.1, capítulo 7: se deshace todo lo escrito desde `beginTransaction`, y el entorno aborta la transacción si `process` termina con ella abierta). Las pruebas de este submódulo, en las dos versiones, usan `SimuladorConTransacciones.groovy`: una versión de jcardsim que guarda los campos y los arrays persistentes del applet al empezar la transacción y los devuelve si se aborta. Los arrays transitorios no los guarda, porque no participan. Es una herramienta de las pruebas: en la tarjeta lo hace el entorno.

**Las pruebas:** `TransaccionesSpec`, solo en el después, con cuatro pruebas que fallan con el código del antes:

| Prueba | Antes | Después |
|---|---|---|
| Un PIN incorrecto en el cambio de PIN | `6F00` | `63C2` |
| Tres cambios de PIN con el PIN incorrecto, y después el PIN correcto | `9000`: los intentos han vuelto | `6983`: bloqueado |
| Un pago de 10 con el registro lleno y 930 de saldo | `6A84`, y el saldo queda en 920 | `6A84`, y el saldo sigue en 930 |
| Una recarga de 500 con el registro lleno y 930 de saldo | `6A84`, y el saldo queda en 1 430 | `6A84`, y el saldo sigue en 930 |

Con jcardsim tal cual, sin el simulador con transacciones, el después también dejaría 920 y 1 430, y el antes no perdería los intentos: el fallo del cambio de PIN solo aparece cuando la transacción se deshace de verdad. Un corte de alimentación a mitad de una escritura no se puede provocar en el simulador; la especificación lo trata igual que una transacción abortada (JCRE 3.1, 7.5).

`TarjetaSpec` cambia en el después lo que cambian los códigos: `63C2`, `63C1` y `63C0` para los tres PIN incorrectos.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
