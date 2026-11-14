import cardcode.eco.EcoApplet
import com.licel.jcardsim.smartcardio.CardSimulator
import com.licel.jcardsim.utils.AIDUtil
import javax.smartcardio.CommandAPDU
import spock.lang.Specification

class EcoSpec extends Specification {

    def tarjeta = new CardSimulator()
    def aid = AIDUtil.create('F000000001')

    def setup() {
        tarjeta.installApplet(aid, EcoApplet)
        tarjeta.selectApplet(aid)
    }

    def 'devuelve los datos que recibe'() {
        when:
        def respuesta = tarjeta.transmitCommand(new CommandAPDU(0x80, 0x10, 0, 0, [1, 2, 3] as byte[]))

        then:
        respuesta.SW == 0x9000
        respuesta.data == [1, 2, 3] as byte[]
    }

    def 'rechaza una instrucción que no conoce'() {
        expect:
        tarjeta.transmitCommand(new CommandAPDU(0x80, 0x11, 0, 0)).SW == 0x6D00
    }
}
