package cardcode.tarjeta;

import javacard.framework.ISOException;
import javacard.framework.Util;

/** El registro, en un array: cada operación ocupa tres bytes, el tipo y el importe. */
public class RegistroOperaciones implements Registro {

    private static final short CAPACIDAD = 8;
    private static final short TAMANO_ENTRADA = 3;

    private final byte[] entradas = new byte[CAPACIDAD * TAMANO_ENTRADA];
    private short ocupadas;

    public void anotar(byte tipo, short importe) {
        if (ocupadas == CAPACIDAD) {
            ISOException.throwIt((short) 0x6A84);
        }
        short posicion = (short) (ocupadas * TAMANO_ENTRADA);
        entradas[posicion] = tipo;
        Util.setShort(entradas, (short) (posicion + 1), importe);
        ocupadas++;
    }

    public short copiar(byte[] destino, short desde) {
        short longitud = (short) (ocupadas * TAMANO_ENTRADA);
        Util.arrayCopyNonAtomic(entradas, (short) 0, destino, desde, longitud);
        return longitud;
    }

    public void vaciar() {
        ocupadas = 0;
    }
}
