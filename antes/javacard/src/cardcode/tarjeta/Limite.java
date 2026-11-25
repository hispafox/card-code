package cardcode.tarjeta;

/** El saldo máximo que admite la tarjeta. */
public class Limite {

    private final int maximo;

    public Limite(int maximo) {
        this.maximo = maximo;
    }

    public int getMaximo() {
        return maximo;
    }
}
