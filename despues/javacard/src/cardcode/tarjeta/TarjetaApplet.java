package cardcode.tarjeta;

import javacard.framework.APDU;
import javacard.framework.Applet;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;
import javacard.framework.Util;

/**
 * La tarjeta: el saldo, la recarga, el pago y el PIN, y el registro de las
 * operaciones que el terminal todavía no ha recogido.
 */
public class TarjetaApplet extends Applet {

    private final Saldo saldo = new Saldo();
    private final Registro registro = new Registro();
    // Todo lo que el applet necesita se crea aquí, al instalarlo.
    private final byte[] pin = new byte[8];
    private boolean personalizada;
    private byte intentos;
    // Lo que solo vale mientras el applet está seleccionado va en memoria
    // transitoria, que se borra al deseleccionarlo y al reiniciar la tarjeta.
    private final byte[] recibido = JCSystem.makeTransientByteArray((short) 8, JCSystem.CLEAR_ON_DESELECT);
    private final boolean[] pinVerificado = JCSystem.makeTransientBooleanArray((short) 1, JCSystem.CLEAR_ON_DESELECT);

    public static void install(byte[] parametros, short desplazamiento, byte longitud) {
        new TarjetaApplet().register();
    }

    public void process(APDU apdu) {
        if (selectingApplet()) {
            return;
        }
        byte[] bufer = apdu.getBuffer();

        switch (bufer[1]) {
        case 0x50: { // personalizar: el PIN de la tarjeta, una sola vez
            if (personalizada) {
                ISOException.throwIt((short) 0x6985);
            }
            short lc = apdu.setIncomingAndReceive();
            if (lc < 4 || lc > 8) {
                ISOException.throwIt((short) 0x6700);
            }
            Util.arrayFillNonAtomic(recibido, (short) 0, (short) 8, (byte) 0xFF);
            Util.arrayCopyNonAtomic(bufer, (short) 5, recibido, (short) 0, lc);
            Util.arrayCopy(recibido, (short) 0, pin, (short) 0, (short) 8);
            intentos = 3;
            personalizada = true;
            return;
        }
        case 0x20: { // verificar el PIN
            short lc = apdu.setIncomingAndReceive();
            if (lc < 4 || lc > 8) {
                ISOException.throwIt((short) 0x6700);
            }
            Util.arrayFillNonAtomic(recibido, (short) 0, (short) 8, (byte) 0xFF);
            Util.arrayCopyNonAtomic(bufer, (short) 5, recibido, (short) 0, lc);
            comprobarPin();
            pinVerificado[0] = true;
            return;
        }
        case 0x24: { // cambiar el PIN: el actual y el nuevo, ocho bytes cada uno
            short lc = apdu.setIncomingAndReceive();
            if (lc != 16) {
                ISOException.throwIt((short) 0x6700);
            }
            Util.arrayCopyNonAtomic(bufer, (short) 5, recibido, (short) 0, (short) 8);
            comprobarPin();
            Util.arrayCopy(bufer, (short) 13, pin, (short) 0, (short) 8);
            return;
        }
        case 0x30: { // pagar
            if (!pinVerificado[0]) {
                ISOException.throwIt((short) 0x6982);
            }
            short lc = apdu.setIncomingAndReceive();
            if (lc != 2) {
                ISOException.throwIt((short) 0x6700);
            }
            short importe = Util.getShort(bufer, (short) 5);
            saldo.descontar(importe);
            registro.anotar((byte) 1, importe);
            return;
        }
        case 0x32: { // recargar
            if (!pinVerificado[0]) {
                ISOException.throwIt((short) 0x6982);
            }
            short lc = apdu.setIncomingAndReceive();
            if (lc != 2) {
                ISOException.throwIt((short) 0x6700);
            }
            short importe = Util.getShort(bufer, (short) 5);
            saldo.cargar(importe);
            registro.anotar((byte) 2, importe);
            return;
        }
        case 0x34: { // consultar el saldo
            Util.setShort(bufer, (short) 0, saldo.getValor());
            apdu.setOutgoingAndSend((short) 0, (short) 2);
            return;
        }
        case (byte) 0xB2: { // leer el registro
            if (!pinVerificado[0]) {
                ISOException.throwIt((short) 0x6982);
            }
            short longitud = registro.copiar(bufer, (short) 0);
            apdu.setOutgoingAndSend((short) 0, longitud);
            return;
        }
        case 0x36: { // vaciar el registro, cuando el terminal ya lo ha recogido
            if (!pinVerificado[0]) {
                ISOException.throwIt((short) 0x6982);
            }
            registro.vaciar();
            return;
        }
        default:
            ISOException.throwIt((short) 0x6D00);
        }
    }

    // El intento se gasta antes de comparar, como en el C del equipo (1.3).
    private void comprobarPin() {
        if (!personalizada || intentos == 0) {
            ISOException.throwIt((short) 0x6983);
        }
        intentos--;
        if (Util.arrayCompare(recibido, (short) 0, pin, (short) 0, (short) 8) != 0) {
            ISOException.throwIt((short) 0x6982);
        }
        intentos = 3;
    }
}
