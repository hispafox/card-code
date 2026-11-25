import spock.lang.Specification

/** Lo que el conversor de Java Card rechazaría, leído del bytecode con javap. */
class SubconjuntoSpec extends Specification {

    def 'no usa clases de Java que la tarjeta no tiene'() {
        when:
        def ajenas = Bytecode.clases().collectEntries { [it.name, Bytecode.clasesDeJava(it)] }.findAll { it.value }

        then:
        ajenas.isEmpty()
    }

    def 'no declara int, long, char, float ni double'() {
        when:
        def ajenos = Bytecode.clases().collectEntries { [it.name, Bytecode.tiposAjenos(it)] }.findAll { it.value }

        then:
        ajenos.isEmpty()
    }

    def 'ningún método que procesa comandos crea objetos'() {
        when:
        def creaciones = Bytecode.clases().collectMany { Bytecode.creacionesAlProcesar(it) }

        then:
        creaciones.isEmpty()
    }
}
