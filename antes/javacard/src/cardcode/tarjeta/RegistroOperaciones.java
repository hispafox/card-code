package cardcode.tarjeta;

import javacard.framework.ISOException;

/** El registro, en un array: cada operación ocupa tres bytes, el tipo y el importe. */
public class RegistroOperaciones implements Registro {

    private static final int CAPACIDAD = 8;
    private static final int TAMANO_ENTRADA = 3;

    private final byte[] entradas = new byte[CAPACIDAD * TAMANO_ENTRADA];
    private int ocupadas;

    public void anotar(byte tipo, int importe) {
        if (ocupadas == CAPACIDAD) {
            throw new ISOException((short) 0x6A84);
        }
        int posicion = ocupadas * TAMANO_ENTRADA;
        entradas[posicion] = tipo;
        entradas[posicion + 1] = (byte) (importe >> 8);
        entradas[posicion + 2] = (byte) importe;
        ocupadas++;
    }

    public int copiar(byte[] destino, int desde) {
        int longitud = ocupadas * TAMANO_ENTRADA;
        System.arraycopy(entradas, 0, destino, desde, longitud);
        return longitud;
    }

    public void vaciar() {
        ocupadas = 0;
    }
}
