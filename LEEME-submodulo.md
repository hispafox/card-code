# Submódulo 3.3 · Tipos que no esconden errores

| Tema · receta | Antes | Después | Dónde |
|---|---|---|---|
| Ancho fijo · hacer explícito el ancho del cálculo | La recarga comprueba `saldo + importe > SALDO_MAXIMO` con dos `uint16_t`: con un `int` de 16 bits, la suma da la vuelta | `(uint32_t)saldo + importe`, y `saldo_nuevo = (uint16_t)(saldo + importe)` después de la comprobación | `c/src/comando.c` |
| Ancho fijo · un tipo de ancho fijo para cada dato con tamaño | `pin_intentos` es un `unsigned int` y se guarda en un byte de la memoria persistente | `uint8_t pin_intentos` | `c/src/pin.c` |
| Conversiones · comprobar antes de estrechar | `comando_anadir(const uint8_t *, uint8_t n)` recibe los 300 bytes de los datos públicos como un `size_t`, y se queda con 44 | `comando_anadir(const uint8_t *, size_t n)` | `c/src/comando.c` |
| Conversiones · pedirle al compilador que avise | — | `-Wconversion` en las opciones del repositorio (`scripts/comun.ps1`, desde `main`). El antes lo quita en `antes/c/compilar.opciones` | `scripts/comun.ps1` |
| `const` · declarar `const` las tablas que no cambian | `uint16_t crc_tabla[256]`, global y sin `const`: sus 512 bytes van a `.data`, y de ahí a la RAM | `static const uint16_t crc_tabla[256]`: a `.rdata` | `c/src/crc.c` |
| `const` · `const` en los punteros de entrada | `pin_verificar(uint8_t *pin, …)` | `pin_verificar(const uint8_t *pin, …)` | `c/include/pin.h`, `c/src/pin.c` |
| `volatile` · en lo que cambia fuera del programa | `escritura_terminada` sin `volatile`: con `-Os`, `memoria_esperar` lee el indicador una vez y salta a sí misma para siempre | `static volatile uint8_t escritura_terminada` | `c/src/memoria.c` |
| `volatile` · leer una vez y trabajar con la copia | `memoria_escribir` lee dos veces el registro de estado | Una lectura a una variable local, y las dos comprobaciones sobre ella | `c/src/memoria.c` |

**Lo que trae la rama para los dos:** la lectura de los datos públicos de la tarjeta, 300 bytes de la memoria persistente (`CA`, GET DATA en ISO/IEC 7816-4, en el módulo `datos`), y por eso la respuesta puede pasar de 255 bytes y su longitud es un `uint16_t`. La simulación del controlador de la memoria: cada escritura pone el registro de estado y llama a `memoria_interrupcion`, que pone el indicador que espera `memoria_esperar`. Los intentos del PIN se guardan en la memoria persistente. El CRC es la versión en C con tabla; esta rama no trae la rutina en ensamblador de 2.4.

**Lo que dice el compilador:** el antes, con `-Wconversion`, da tres avisos, el de `comando_anadir` y dos de `pin_intentos`. Si la longitud que se pasa a `comando_anadir` fuera un `sizeof` (un valor que se conoce al compilar), gcc avisaría sin `-Wconversion`, con `-Woverflow`, que está activo por defecto: el estrechamiento callado es el de un valor que solo se sabe al ejecutar. El ensamblador de `memoria_esperar` se ve con `gcc -Os -S` sobre `memoria.c` de cada versión.

**Las pruebas:** el después trae una recarga de 30.000 sobre 40.000 (`6A 80`; en el portátil pasa también en el antes), los datos públicos que salen enteros (302 bytes con el código de estado; el antes da 46) y la espera que termina. Verificar el PIN escribe ahora en la memoria, así que la prueba del cambio de PIN hace fallar la escritura de la longitud del PIN (`memoria_fallar_escritura_en`).

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
