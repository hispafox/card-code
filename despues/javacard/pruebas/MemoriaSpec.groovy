import com.licel.jcardsim.utils.AIDUtil
import javacard.framework.JCSystem

class MemoriaSpec extends TarjetaSimulada {

    def 'después de reiniciar la tarjeta hay que volver a verificar el PIN'() {
        given:
        verificar()

        when:
        tarjeta.reset()
        tarjeta.selectApplet(aid)

        then:
        recargar(100).SW == 0x6982
    }

    def 'después de seleccionar otro applet hay que volver a verificar el PIN'() {
        given:
        def otro = AIDUtil.create('F000000003')
        tarjeta.installApplet(otro, OtroApplet)
        verificar()

        when:
        tarjeta.selectApplet(otro)
        tarjeta.selectApplet(aid)

        then:
        recargar(100).SW == 0x6982
    }

    def 'el PIN recibido y la sesión están en memoria transitoria'() {
        given:
        def memoria = simulador.transientMemory

        expect:
        memoria.isTransient(applet().@recibido) == JCSystem.CLEAR_ON_DESELECT
        memoria.isTransient(applet().@pinVerificado) == JCSystem.CLEAR_ON_DESELECT
        memoria.isTransient(applet().@pin) == JCSystem.NOT_A_TRANSIENT_OBJECT
    }

    def 'el PIN recibido no queda en la tarjeta al deseleccionarla'() {
        given:
        def otro = AIDUtil.create('F000000003')
        tarjeta.installApplet(otro, OtroApplet)
        verificar(PIN_INCORRECTO)

        when:
        tarjeta.selectApplet(otro)

        then:
        applet().@recibido.every { it == 0 }
    }
}
