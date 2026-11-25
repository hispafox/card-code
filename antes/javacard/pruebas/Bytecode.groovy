import java.util.spi.ToolProvider

/**
 * Lee el bytecode de las clases del applet con javap, la herramienta del JDK,
 * para comprobar lo que el conversor de Java Card rechazaría. Es una
 * aproximación: la última palabra la tiene el conversor del kit de Oracle.
 */
class Bytecode {

    static List<File> clases() {
        def carpeta = new File(cardcode.tarjeta.TarjetaApplet.getResource('TarjetaApplet.class').toURI()).parentFile
        carpeta.listFiles().findAll { it.name.endsWith('.class') }.sort { it.name }
    }

    static String javap(File clase, String... opciones) {
        def salida = new StringWriter()
        def errores = new StringWriter()
        def argumentos = (opciones.toList() + [clase.path]) as String[]
        int codigo = ToolProvider.findFirst('javap').get().run(new PrintWriter(salida), new PrintWriter(errores), argumentos)
        assert codigo == 0 : errores.toString()
        salida.toString()
    }

    /** Lo que Java Card trae de java.lang: Object, Throwable y las excepciones de la máquina virtual. */
    static final Set<String> JAVA_LANG_DE_JAVA_CARD = ['Object', 'Throwable', 'Exception', 'RuntimeException',
            'ArithmeticException', 'ArrayIndexOutOfBoundsException', 'ArrayStoreException', 'ClassCastException',
            'IndexOutOfBoundsException', 'NegativeArraySizeException', 'NullPointerException',
            'SecurityException'].collect { 'java/lang/' + it }.toSet()

    /** Las clases de java.* que usa y que Java Card no tiene. */
    static Set<String> clasesDeJava(File clase) {
        (javap(clase, '-c', '-p') =~ /java[.\/](?:[a-z]\w*[.\/])+[A-Z][\w$]*/).collect { it.replace('.', '/') }
                .findAll { !(it in JAVA_LANG_DE_JAVA_CARD) }.toSet()
    }

    /** Campos y métodos declarados con tipos que Java Card no tiene o que son opcionales. */
    static List<String> tiposAjenos(File clase) {
        javap(clase, '-p').readLines().findAll { it =~ /\b(int|long|char|float|double)\b/ }*.trim()
    }

    /**
     * Las instrucciones que crean objetos (new, newarray, anewarray) o arrays
     * transitorios, fuera del constructor, del inicializador de la clase y de install.
     */
    static List<String> creacionesAlProcesar(File clase) {
        def encontradas = []
        String metodo = null
        javap(clase, '-c', '-p').readLines().each { linea ->
            def cabecera = linea =~ /^  (?!\s)(.*)\);$/
            if (cabecera) {
                metodo = (cabecera[0][1] =~ /([\w.$<>]+)\(/)[0][1]
            } else if (linea ==~ /^  static \{\};$/) {
                metodo = '<clinit>'
            } else if (linea =~ /: (new|newarray|anewarray|multianewarray)\b/ || linea.contains('makeTransient')) {
                boolean alInstalar = metodo == null || metodo.contains('.') || metodo in ['install', '<clinit>']
                if (!alInstalar) {
                    encontradas << "${clase.name} · ${metodo} · ${linea.trim()}"
                }
            }
        }
        encontradas
    }
}
