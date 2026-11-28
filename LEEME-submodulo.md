# Submódulo 1.1 · Legible, rápido y pequeño a la vez

| Tema | Antes | Después | Dónde |
|---|---|---|---|
| Cuando las tres van juntas | La comprobación de la longitud copiada en los tres comandos, escrita de tres maneras; la del pago, sin la comprobación de tamaño, porque el arreglo no le llegó | Una sola función, `datos_copiar`, que llaman los tres | `c/src/comandos.c` |
| Cuando tiran cada una hacia un lado | El CRC de 16 bits calculado bit a bit | El CRC con una tabla de 256 valores ya calculados | `c/src/crc16.c` |

Las pruebas del después incluyen el pago con datos de más, el caso que en el antes nadie había probado.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
