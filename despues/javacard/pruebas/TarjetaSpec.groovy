class TarjetaSpec extends TarjetaSimulada {

    def 'se personaliza una sola vez'() {
        expect:
        enviar(0x50, PIN_INCORRECTO).SW == 0x6985
        verificar().SW == 0x9000
    }

    def 'rechaza un PIN de longitud incorrecta'() {
        expect:
        verificar([1, 2, 3] as byte[]).SW == 0x6700
    }

    def 'rechaza un PIN incorrecto con los intentos que quedan'() {
        expect:
        verificar(PIN_INCORRECTO).SW == 0x63C2
    }

    def 'se bloquea al tercer PIN incorrecto'() {
        expect:
        verificar(PIN_INCORRECTO).SW == 0x63C2
        verificar(PIN_INCORRECTO).SW == 0x63C1
        verificar(PIN_INCORRECTO).SW == 0x63C0
        verificar().SW == 0x6983
    }

    def 'un PIN correcto devuelve los intentos'() {
        when:
        2.times { verificar(PIN_INCORRECTO) }
        verificar()
        2.times { verificar(PIN_INCORRECTO) }

        then:
        verificar().SW == 0x9000
    }

    def 'sin el PIN no paga ni recarga'() {
        expect:
        pagar(100).SW == 0x6982
        recargar(100).SW == 0x6982
    }

    def 'recarga, paga y consulta el saldo'() {
        given:
        verificar()

        expect:
        recargar(1000).SW == 0x9000
        pagar(250).SW == 0x9000
        saldo() == 750
    }

    def 'no paga más de lo que hay'() {
        given:
        verificar()
        recargar(100)

        expect:
        pagar(101).SW == 0x6985
        saldo() == 100
    }

    def 'rechaza un importe cero'() {
        given:
        verificar()

        expect:
        pagar(0).SW == 0x6A80
        recargar(0).SW == 0x6A80
    }

    def 'no recarga por encima del saldo máximo'() {
        given:
        verificar()

        expect:
        recargar(30000).SW == 0x9000
        recargar(1).SW == 0x6A80
        saldo() == 30000
    }

    def 'rechaza un importe que no cabe en un short'() {
        given:
        verificar()

        expect:
        recargar(40000).SW == 0x6A80
        saldo() == 0
    }

    def 'lee el registro'() {
        given:
        verificar()
        recargar(1000)
        pagar(250)

        when:
        def respuesta = pedir(0xB2)

        then:
        respuesta.SW == 0x9000
        respuesta.data == [2, 0x03, 0xE8, 1, 0x00, 0xFA] as byte[]
    }

    def 'rechaza el pago cuando el registro está lleno'() {
        given:
        verificar()
        recargar(1000)
        7.times { pagar(10) }

        expect:
        pagar(10).SW == 0x6A84
    }

    def 'con el registro vaciado vuelve a pagar'() {
        given:
        verificar()
        recargar(1000)
        7.times { pagar(10) }

        expect:
        enviar(0x36, new byte[0]).SW == 0x9000
        pagar(10).SW == 0x9000
    }

    def 'cambia el PIN'() {
        given:
        def nuevo = [5, 6, 7, 8] as byte[]

        expect:
        cambiarPin(PIN, nuevo).SW == 0x9000
        verificar(PIN).SW == 0x63C2
        verificar(nuevo).SW == 0x9000
    }

    def 'rechaza una instrucción que no conoce'() {
        expect:
        enviar(0x40, new byte[0]).SW == 0x6D00
    }
}
