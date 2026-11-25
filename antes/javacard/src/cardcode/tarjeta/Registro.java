package cardcode.tarjeta;

/** Las operaciones que el terminal todavía no ha recogido. */
public interface Registro {

    void anotar(byte tipo, int importe);

    int copiar(byte[] destino, int desde);

    void vaciar();
}
