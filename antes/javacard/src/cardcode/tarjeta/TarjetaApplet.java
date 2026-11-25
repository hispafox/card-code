package cardcode.tarjeta;

import java.util.Arrays;

import javacard.framework.APDU;
import javacard.framework.Applet;
import javacard.framework.ISOException;

/**
 * La tarjeta: el saldo, la recarga, el pago y el PIN, y el registro de las
 * operaciones que el terminal todavía no ha recogido.
 */
public class TarjetaApplet extends Applet {

    private final Saldo saldo = new Saldo();
    private final Limite limite = new Limite(50000);
    private final Registro registro = new RegistroOperaciones();
    private byte[] pin;
    private int intentos;
    private boolean pinVerificado;

    public static void install(byte[] parametros, short desplazamiento, byte longitud) {
        new TarjetaApplet().register();
    }

    public void process(APDU apdu) {
        if (selectingApplet()) {
            return;
        }
        byte[] bufer = apdu.getBuffer();
        int ins = bufer[1];
        System.out.println("Comando " + Integer.toHexString(ins) + ", saldo " + saldo.getValor());

        switch (ins) {
        case 0x50: { // personalizar: el PIN de la tarjeta, una sola vez
            if (pin != null) {
                throw new ISOException((short) 0x6985);
            }
            int lc = apdu.setIncomingAndReceive();
            if (lc < 4 || lc > 8) {
                throw new ISOException((short) 0x6700);
            }
            pin = new byte[8];
            Arrays.fill(pin, (byte) 0xFF);
            System.arraycopy(bufer, 5, pin, 0, lc);
            intentos = 3;
            return;
        }
        case 0x20: { // verificar el PIN
            int lc = apdu.setIncomingAndReceive();
            if (lc < 4 || lc > 8) {
                throw new ISOException((short) 0x6700);
            }
            byte[] recibido = new byte[8];
            Arrays.fill(recibido, (byte) 0xFF);
            System.arraycopy(bufer, 5, recibido, 0, lc);
            comprobarPin(recibido);
            pinVerificado = true;
            return;
        }
        case 0x24: { // cambiar el PIN: el actual y el nuevo, ocho bytes cada uno
            int lc = apdu.setIncomingAndReceive();
            if (lc != 16) {
                throw new ISOException((short) 0x6700);
            }
            byte[] actual = new byte[8];
            System.arraycopy(bufer, 5, actual, 0, 8);
            comprobarPin(actual);
            System.arraycopy(bufer, 13, pin, 0, 8);
            return;
        }
        case 0x30: { // pagar
            if (!pinVerificado) {
                throw new ISOException((short) 0x6982);
            }
            int lc = apdu.setIncomingAndReceive();
            if (lc != 2) {
                throw new ISOException((short) 0x6700);
            }
            int importe = ((bufer[5] & 0xFF) << 8) | (bufer[6] & 0xFF);
            if (importe == 0) {
                throw new ISOException((short) 0x6A80);
            }
            if (importe > saldo.getValor()) {
                throw new ISOException((short) 0x6985);
            }
            saldo.setValor(saldo.getValor() - importe);
            registro.anotar((byte) 1, importe);
            return;
        }
        case 0x32: { // recargar
            if (!pinVerificado) {
                throw new ISOException((short) 0x6982);
            }
            int lc = apdu.setIncomingAndReceive();
            if (lc != 2) {
                throw new ISOException((short) 0x6700);
            }
            int importe = ((bufer[5] & 0xFF) << 8) | (bufer[6] & 0xFF);
            if (importe == 0 || saldo.getValor() + importe > limite.getMaximo()) {
                throw new ISOException((short) 0x6A80);
            }
            saldo.setValor(saldo.getValor() + importe);
            registro.anotar((byte) 2, importe);
            return;
        }
        case 0x34: { // consultar el saldo
            byte[] respuesta = new byte[2];
            respuesta[0] = (byte) (saldo.getValor() >> 8);
            respuesta[1] = (byte) saldo.getValor();
            System.arraycopy(respuesta, 0, bufer, 0, 2);
            apdu.setOutgoingAndSend((short) 0, (short) 2);
            return;
        }
        case 0xB2: { // leer el registro
            if (!pinVerificado) {
                throw new ISOException((short) 0x6982);
            }
            int longitud = registro.copiar(bufer, 0);
            apdu.setOutgoingAndSend((short) 0, (short) longitud);
            return;
        }
        case 0x36: { // vaciar el registro, cuando el terminal ya lo ha recogido
            if (!pinVerificado) {
                throw new ISOException((short) 0x6982);
            }
            registro.vaciar();
            return;
        }
        default:
            throw new ISOException((short) 0x6D00);
        }
    }

    // El intento se gasta antes de comparar, como en el C del equipo (1.3).
    private void comprobarPin(byte[] recibido) {
        if (pin == null || intentos == 0) {
            throw new ISOException((short) 0x6983);
        }
        intentos--;
        if (!Arrays.equals(recibido, pin)) {
            throw new ISOException((short) 0x6982);
        }
        intentos = 3;
    }
}
