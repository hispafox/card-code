# Submódulo 4.1 · Lo que cambia respecto a Java

El applet de la tarjeta (`javacard/src/cardcode/tarjeta/`): saldo, recarga, pago, PIN y un registro de las operaciones que el terminal todavía no ha recogido. **El antes** está escrito con los hábitos de Java: compila con `javac` y sus pruebas pasan en el simulador, pero el conversor de Java Card lo rechazaría. **El después** es el mismo applet dentro del subconjunto de Java Card.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Sin cadenas ni biblioteca estándar · quitar las trazas de la tarjeta | `System.out.println("Comando " + Integer.toHexString(ins) + …)` al principio de `process` | Sin trazas: lo que pasa lo cuentan las pruebas | `TarjetaApplet.java` |
| Sin cadenas ni biblioteca estándar · las utilidades de `Util` | `System.arraycopy`, `Arrays.fill` y `Arrays.equals` | `Util.arrayCopy`, `Util.arrayCopyNonAtomic`, `Util.arrayFillNonAtomic` y `Util.arrayCompare` | `TarjetaApplet.java`, `RegistroOperaciones.java` |
| `short` y `byte` · bajar a `short` y `byte`, con su conversión | `int` en el saldo, el importe, los intentos y el registro; `int ins = bufer[1]` con `case 0xB2`, que nunca coincide | `short` y `byte`; `case (byte) 0xB2`; `(short) (a - b)`; el importe que llega negativo se rechaza; el límite se comprueba con una resta | todas las clases |
| `short` y `byte` · leer y escribir los `short` con `Util` | El importe montado con desplazamientos y máscaras; el saldo, en un array nuevo y copiado al búfer | `Util.getShort` y `Util.setShort`, sobre el búfer de la APDU | `TarjetaApplet.java`, `RegistroOperaciones.java` |
| Los objetos, al instalar · reservar en la instalación | `new byte[8]` al personalizar, al verificar y al cambiar el PIN, y `new byte[2]` al consultar; `pin == null` como «sin personalizar» | `pin` y `recibido` creados con el applet; `personalizada` | `TarjetaApplet.java` |
| Los objetos, al instalar · lanzar con `throwIt` | `throw new ISOException(…)`, diecisiete veces | `ISOException.throwIt(…)` | `TarjetaApplet.java`, `RegistroOperaciones.java` |

**El saldo máximo baja de 50 000 a 30 000.** En Java Card no hay tipos sin signo, y un `short` llega hasta 32 767. Por la misma razón, un importe de más de 32 767 llega negativo, y el después lo rechaza.

**Lo que no es de este submódulo se queda como está:** las clases `Saldo` (solo guarda el valor), `Limite` y la interfaz `Registro` con una sola implementación son de 4.2; el búfer `recibido` en memoria persistente y `pinVerificado` en un campo, de 4.3; el `switch` con toda la lógica dentro y los números sueltos, de 4.4; el pago que escribe el saldo antes de saber si cabe en el registro, de 4.5.

**Las pruebas:** `TarjetaSpec` comprueba el comportamiento. El después trae tres pruebas nuevas que fallan con el código del antes: `lee el registro` (el antes responde `6D00`), `rechaza un importe que no cabe en un short` y el saldo máximo de 30 000. Y `SubconjuntoSpec`, que lee el bytecode con `javap` (la herramienta del JDK, sin conexión) y comprueba tres cosas: que no se usa ninguna clase de `java.*` salvo `java.lang.Object`, que no se declara nada `int`, `long`, `char`, `float` ni `double`, y que ningún método fuera del constructor y de `install` crea objetos. Con el código del antes da siete clases de Java (`System`, `String`, `StringBuilder`, `Integer`, `PrintStream`, `Arrays`, y `System` otra vez en el registro), declaraciones `int` en las cinco clases y veintidós objetos creados al procesar comandos. **Es una aproximación**: la última palabra la tiene el conversor del kit de Oracle, que no va en el repositorio.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
