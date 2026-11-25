# Submódulo 4.2 · Clases pequeñas con lo que cuesta cada objeto

**El antes** es el después de 4.1 tal cual: el applet dentro del subconjunto de Java Card, con las clases que trajo de Java. **El después** reparte el trabajo en dos clases que hacen algo y quita las que no compensan lo que cuestan. El comportamiento no cambia: las mismas pruebas, en verde en las dos versiones.

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Clases que hacen su trabajo · mover el método a la clase de sus datos | `Saldo` solo guarda el valor (`getValor` y `setValor`); las reglas del pago y de la recarga están en `process` | `Saldo.descontar` y `Saldo.cargar`, con sus comprobaciones dentro; `setValor` desaparece | `Saldo.java`, `TarjetaApplet.java` |
| Lo que cuesta cada objeto · integrar la clase que no compensa | `Limite`, una clase y un objeto para guardar un `short` que no cambia | `Saldo.MAXIMO`, una constante | `Saldo.java` (y `Limite.java` desaparece) |
| Lo que cuesta cada llamada · quitar la interfaz con una sola implementación | La interfaz `Registro` y su única implementación, `RegistroOperaciones`; las llamadas van por la interfaz | Una clase, `Registro` | `Registro.java` (y `RegistroOperaciones.java` desaparece) |

**Medido con `javap -c`** sobre `TarjetaApplet.class` (bytecode de Java, no el del conversor):

| | Antes | Después |
|---|---|---|
| Llamadas de `TarjetaApplet` a métodos de sus propias clases, sin contar los constructores | 12 (4 por interfaz) | 7 (ninguna por interfaz) |
| Llamadas en el camino del pago | 4: `getValor` dos veces, `setValor` y `anotar` por la interfaz | 2: `descontar` y `anotar` |
| Llamadas en el camino de la recarga | 5: `getMaximo`, `getValor` dos veces, `setValor` y `anotar` | 2: `cargar` y `anotar` |
| Tipos del paquete | 5 (cuatro clases y una interfaz) | 3 clases |
| Objetos que se crean al instalar | 7: el applet, `Saldo`, `Limite`, `RegistroOperaciones` y tres arrays | 6: el applet, `Saldo`, `Registro` y tres arrays |

El tamaño del applet en la tarjeta lo da el fichero CAP que genera el conversor del kit de Oracle, que no va en el repositorio.

**Lo que no es de este submódulo se queda como está:** el búfer `recibido` y `pinVerificado` en memoria persistente (4.3), el `switch` con la lógica de cada comando dentro y los números sueltos (4.4), y el pago que escribe el saldo antes de saber si cabe en el registro (4.5).

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
```
