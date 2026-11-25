package cardcode.tarjeta;

/** Las operaciones que el terminal todavía no ha recogido. */
public interface Registro {

    void anotar(byte tipo, short importe);

    short copiar(byte[] destino, short desde);

    void vaciar();
}
