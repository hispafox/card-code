import javacard.framework.APDU
import javacard.framework.Applet

/** Otro applet de la misma tarjeta, de otro paquete, para seleccionarlo entre dos comandos. */
class OtroApplet extends Applet {

    static void install(byte[] parametros, short desplazamiento, byte longitud) {
        new OtroApplet().register()
    }

    void process(APDU apdu) {
    }
}
