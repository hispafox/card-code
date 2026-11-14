package cardcode.eco;

import javacard.framework.APDU;
import javacard.framework.Applet;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;

/** Applet mínimo para comprobar el entorno: devuelve los datos que recibe. */
public class EcoApplet extends Applet {

    private static final byte INS_ECO = (byte) 0x10;

    public static void install(byte[] parametros, short desplazamiento, byte longitud) {
        new EcoApplet().register();
    }

    public void process(APDU apdu) {
        if (selectingApplet()) {
            return;
        }
        byte[] bufer = apdu.getBuffer();
        if (bufer[ISO7816.OFFSET_INS] != INS_ECO) {
            ISOException.throwIt(ISO7816.SW_INS_NOT_SUPPORTED);
        }
        short longitud = apdu.setIncomingAndReceive();
        apdu.setOutgoingAndSend(ISO7816.OFFSET_CDATA, longitud);
    }
}
