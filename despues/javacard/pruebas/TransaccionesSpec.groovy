class TransaccionesSpec extends TarjetaSimulada {

    def 'un PIN incorrecto en el cambio de PIN responde con los intentos que quedan'() {
        expect:
        cambiarPin(PIN_INCORRECTO, [5, 6, 7, 8] as byte[]).SW == 0x63C2
    }

    def 'el cambio de PIN también gasta los intentos'() {
        when:
        3.times { cambiarPin(PIN_INCORRECTO, [5, 6, 7, 8] as byte[]) }

        then:
        verificar().SW == 0x6983
    }

    def 'un pago que no cabe en el registro no toca el saldo'() {
        given:
        verificar()
        recargar(1000)
        7.times { pagar(10) }

        expect:
        pagar(10).SW == 0x6A84
        saldo() == 930
    }

    def 'una recarga que no cabe en el registro no toca el saldo'() {
        given:
        verificar()
        recargar(1000)
        7.times { pagar(10) }

        expect:
        recargar(500).SW == 0x6A84
        saldo() == 930
    }
}
