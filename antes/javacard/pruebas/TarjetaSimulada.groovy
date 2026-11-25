import cardcode.tarjeta.TarjetaApplet
import com.licel.jcardsim.smartcardio.CardSimulator
import com.licel.jcardsim.utils.AIDUtil
import javax.smartcardio.CommandAPDU
import javax.smartcardio.ResponseAPDU
import spock.lang.Specification

/** La tarjeta en el simulador, ya personalizada, y los comandos que le manda el terminal. */
abstract class TarjetaSimulada extends Specification {

    static final byte[] PIN = [1, 2, 3, 4] as byte[]
    static final byte[] PIN_INCORRECTO = [9, 9, 9, 9] as byte[]

    def aid = AIDUtil.create('F000000002')
    // El simulador que deshace las transacciones, como la tarjeta.
    def simulador = new SimuladorConTransacciones()
    def tarjeta = new CardSimulator(simulador)

    def setup() {
        tarjeta.installApplet(aid, TarjetaApplet)
        tarjeta.selectApplet(aid)
        assert enviar(0x50, PIN).SW == 0x9000
    }

    /** El objeto del applet, para mirar dónde están sus datos. */
    TarjetaApplet applet() {
        (TarjetaApplet) simulador.lookupApplet(aid).getApplet()
    }

    ResponseAPDU enviar(int ins, byte[] datos) {
        tarjeta.transmitCommand(new CommandAPDU(0x80, ins, 0, 0, datos))
    }

    ResponseAPDU pedir(int ins) {
        tarjeta.transmitCommand(new CommandAPDU(0x80, ins, 0, 0, 256))
    }

    static byte[] importe(int valor) {
        [(byte) (valor >> 8), (byte) valor] as byte[]
    }

    static byte[] relleno(byte[] pin) {
        def ocho = new byte[8]
        Arrays.fill(ocho, (byte) 0xFF)
        System.arraycopy(pin, 0, ocho, 0, pin.length)
        ocho
    }

    // El cambio de PIN lleva el actual y el nuevo, rellenos hasta ocho bytes.
    static byte[] cambio(byte[] actual, byte[] nuevo) {
        (relleno(actual).toList() + relleno(nuevo).toList()) as byte[]
    }

    ResponseAPDU verificar(byte[] pin = PIN) { enviar(0x20, pin) }
    ResponseAPDU cambiarPin(byte[] actual, byte[] nuevo) { enviar(0x24, cambio(actual, nuevo)) }
    ResponseAPDU pagar(int valor) { enviar(0x30, importe(valor)) }
    ResponseAPDU recargar(int valor) { enviar(0x32, importe(valor)) }
    int saldo() { def r = pedir(0x34); assert r.SW == 0x9000; ((r.data[0] & 0xFF) << 8) | (r.data[1] & 0xFF) }
}
