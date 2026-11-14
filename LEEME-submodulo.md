# Submódulo 1.2 · Lo que dicen las métricas

| Antes | Después | Dónde |
|---|---|---|
| `registro_escribir` con cuatro niveles de anidamiento y una sola salida (ciclomática 6, cognitiva 18) | Los errores salen en cuanto se detectan y la escritura pasa a `datos_grabar` (cognitiva 3 en cada función) | `c/src/registro.c` |

Las seis pruebas de `c/pruebas/prueba_registro.c` son las mismas en las dos versiones y cubren los seis caminos: PIN sin verificar, registro que no existe, datos que no caben, longitud cero, escritura correcta y escritura que falla.

```powershell
.\scripts\probar.ps1 -Version antes
.\scripts\probar.ps1 -Version despues
.\scripts\medir-c.ps1
```
