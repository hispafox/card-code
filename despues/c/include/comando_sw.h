#ifndef COMANDO_SW_H
#define COMANDO_SW_H

/* Códigos de estado de la respuesta, con los valores de ISO/IEC 7816-4. */
#define COMANDO_SW_SIN_ERROR                0x9000u
#define COMANDO_SW_LONGITUD_INCORRECTA      0x6700u
#define COMANDO_SW_METODO_BLOQUEADO         0x6983u
#define COMANDO_SW_SEGURIDAD_NO_SATISFECHA  0x6982u
#define COMANDO_SW_SALDO_INSUFICIENTE       0x6985u
#define COMANDO_SW_INS_NO_ADMITIDA          0x6D00u

#endif /* COMANDO_SW_H */
