/*
 * CRC-16 (polinomio 0x1021, valor inicial 0xFFFF), bit a bit, para el
 * procesador del portátil (x86-64). Lo llama solo crc.c.
 *
 *   uint16_t crc_calcular_x64(const uint8_t *datos, size_t longitud);
 *
 * Los parámetros llegan en rcx y rdx en Windows, y en rdi y rsi en Linux.
 */
#if defined(CRC_CON_ENSAMBLADOR)

        .text
        .globl  crc_calcular_x64
crc_calcular_x64:
#if defined(_WIN32)
        movq    %rcx, %r8               /* r8 = datos */
        movq    %rdx, %r9               /* r9 = longitud */
#else
        movq    %rdi, %r8
        movq    %rsi, %r9
#endif
        movl    $0xFFFF, %eax           /* crc = 0xFFFF */
        testq   %r9, %r9
        jz      9f
1:      movzbl  (%r8), %ecx             /* crc ^= byte << 8 */
        shll    $8, %ecx
        xorl    %ecx, %eax
        movl    $8, %edx                /* ocho bits por byte */
2:      shll    $1, %eax
        testl   $0x10000, %eax          /* ¿se ha salido un 1 por arriba? */
        jz      3f
        xorl    $0x1021, %eax
3:      andl    $0xFFFF, %eax
        decl    %edx
        jnz     2b
        incq    %r8
        decq    %r9
        jnz     1b
9:      ret

#endif
