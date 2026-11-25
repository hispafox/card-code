# Submódulo 4.4 · El procesado de comandos APDU

**Práctica del bloque 4:** reorganizar el despacho de comandos del applet sin cambiar su comportamiento. **El antes** es el después de 4.3 tal cual: un `process` de 93 líneas con la lógica de los ocho comandos dentro del `switch` y los números sueltos. **El después** separa el despacho de la lógica y pone nombre a cada número. Las pruebas son las mismas en las dos versiones, y pasan en las dos: eso es lo que dice que el comportamiento no ha cambiado.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| El despacho, separado de la lógica · extraer un método por comando | Cada `case` lleva su comando entero, entre llaves | Cada `case` llama a un método: `personalizar`, `verificarPin`, `cambiarPin`, `pagar`, `recargar`, `consultar`, `leerRegistro` y `vaciarRegistro` | `TarjetaApplet.java` |
| El despacho, separado de la lógica · sacar a un método lo que comparten los comandos | La comprobación del PIN verificado, repetida en cuatro comandos; la recepción y la longitud, en seis | `exigirPinVerificado`, `recibir`, `recibirImporte` y `recibirPin` | `TarjetaApplet.java` |
| Constantes con nombre · las constantes de `ISO7816` | `bufer[1]`, `(short) 5`, `(short) 0x6982`, `(short) 0x6700`… | `ISO7816.OFFSET_INS`, `ISO7816.OFFSET_CDATA`, `ISO7816.SW_SECURITY_STATUS_NOT_SATISFIED`, `ISO7816.SW_WRONG_LENGTH`… | todas las clases |
| Constantes con nombre · nombrar las instrucciones y las longitudes propias | `case 0x30`, `lc != 16`, `intentos = 3`… | `INS_PAGAR`, `LONGITUD_CAMBIO_PIN`, `PIN_INTENTOS`…; `SW_PIN_BLOQUEADO` para el `0x6983`, que la API 3.0.5 no trae con ese nombre | `TarjetaApplet.java`, `Registro.java` |

**Medido sobre el código:**

| | Antes | Después |
|---|---|---|
| Líneas de `process` | 93 | 19 |
| Complejidad ciclomática de `process`, contada a mano con las reglas de 1.2 | 22 | 10 |
| Números hexadecimales en el código del applet | 27, repartidos por el código | 10, todos en declaraciones de constantes |
| Líneas de `TarjetaApplet.java` | 135 | 173 |

**Las pruebas:** las mismas 23 en el antes y en el después, todas en verde.

**Lo que no es de este submódulo se queda como está:** el pago que escribe el saldo antes de saber si cabe en el registro, y los códigos de estado de un PIN incorrecto (4.5).

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
