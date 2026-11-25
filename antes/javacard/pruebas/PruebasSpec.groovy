import cardcode.tarjeta.TarjetaApplet
import com.licel.jcardsim.smartcardio.CardSimulator
import com.licel.jcardsim.utils.AIDUtil
import javax.smartcardio.CommandAPDU
import spock.lang.Specification

class PruebasSpec extends Specification {

    def test1() {
        setup:
        def sim = new CardSimulator(new SimuladorConTransacciones())
        def aid = AIDUtil.create('F000000002')
        sim.installApplet(aid, TarjetaApplet)
        sim.selectApplet(aid)
        // personalizar con 1234
        def r = sim.transmitCommand(new CommandAPDU(0x80, 0x50, 0, 0, [1, 2, 3, 4] as byte[]))
        assert r.getSW() == 36864
        // otra vez no se puede
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x50, 0, 0, [9, 9, 9, 9] as byte[]))
        assert r.getSW() == 0x6985
        // longitud mal
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [1, 2, 3] as byte[]))
        assert r.getSW() == 0x6700
        // tres veces mal y se bloquea
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [9, 9, 9, 9] as byte[]))
        assert r.getSW() == 0x63C2
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [9, 9, 9, 9] as byte[]))
        assert r.getSW() == 0x63C1
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [9, 9, 9, 9] as byte[]))
        assert r.getSW() == 0x63C0
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [1, 2, 3, 4] as byte[]))
        assert r.getSW() == 0x6983
        // instruccion que no existe
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x40, 0, 0, new byte[0]))
        assert r.getSW() == 0x6D00
    }

    def test2() {
        setup:
        def sim = new CardSimulator(new SimuladorConTransacciones())
        def aid = AIDUtil.create('F000000002')
        sim.installApplet(aid, TarjetaApplet)
        sim.selectApplet(aid)
        def r = sim.transmitCommand(new CommandAPDU(0x80, 0x50, 0, 0, [1, 2, 3, 4] as byte[]))
        assert r.getSW() == 36864
        // sin pin
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x30, 0, 0, [0, 100] as byte[]))
        assert r.getSW() == 0x6982
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0, 100] as byte[]))
        assert r.getSW() == 0x6982
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [1, 2, 3, 4] as byte[]))
        assert r.getSW() == 36864
        // 1000 y 250
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0x03, 0xE8] as byte[]))
        assert r.getSW() == 36864
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x30, 0, 0, [0x00, 0xFA] as byte[]))
        assert r.getSW() == 36864
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x34, 0, 0, 256))
        assert r.getData()[0] == 2 && (r.getData()[1] & 0xFF) == 0xEE
        // mas de lo que hay
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x30, 0, 0, [0x03, 0x00] as byte[]))
        assert r.getSW() == 0x6985
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x34, 0, 0, 256))
        assert r.getData()[0] == 2 && (r.getData()[1] & 0xFF) == 0xEE
    }

    def test3() {
        setup:
        def sim = new CardSimulator(new SimuladorConTransacciones())
        def aid = AIDUtil.create('F000000002')
        sim.installApplet(aid, TarjetaApplet)
        sim.selectApplet(aid)
        def r = sim.transmitCommand(new CommandAPDU(0x80, 0x50, 0, 0, [1, 2, 3, 4] as byte[]))
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [1, 2, 3, 4] as byte[]))
        // importes que no valen
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x30, 0, 0, [0, 0] as byte[]))
        assert r.getSW() == 0x6A80
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0, 0] as byte[]))
        assert r.getSW() == 0x6A80
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0x75, 0x31] as byte[]))
        assert r.getSW() == 0x6A80
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0x9C, 0x40] as byte[]))
        assert r.getSW() == 0x6A80
    }

    def test4() {
        setup:
        def sim = new CardSimulator(new SimuladorConTransacciones())
        def aid = AIDUtil.create('F000000002')
        sim.installApplet(aid, TarjetaApplet)
        sim.selectApplet(aid)
        def r = sim.transmitCommand(new CommandAPDU(0x80, 0x50, 0, 0, [1, 2, 3, 4] as byte[]))
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x20, 0, 0, [1, 2, 3, 4] as byte[]))
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0x03, 0xE8] as byte[]))
        // llenar el registro (8 con la recarga)
        for (int i = 0; i < 7; i++) {
            r = sim.transmitCommand(new CommandAPDU(0x80, 0x30, 0, 0, [0, 10] as byte[]))
        }
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x30, 0, 0, [0, 10] as byte[]))
        assert r.getSW() == 0x6A84
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x32, 0, 0, [0x01, 0xF4] as byte[]))
        assert r.getSW() == 0x6A84
        r = sim.transmitCommand(new CommandAPDU(0x80, 0x34, 0, 0, 256))
        assert ((r.getData()[0] & 0xFF) << 8 | (r.getData()[1] & 0xFF)) == 930
    }
}
