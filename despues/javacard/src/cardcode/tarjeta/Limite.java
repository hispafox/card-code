package cardcode.tarjeta;

/** El saldo máximo que admite la tarjeta. */
public class Limite {

    private final short maximo;

    public Limite(short maximo) {
        this.maximo = maximo;
    }

    public short getMaximo() {
        return maximo;
    }
}
