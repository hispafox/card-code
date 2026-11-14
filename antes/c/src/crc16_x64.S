/*
 * CRC-16 (polinomio 0x1021, valor inicial 0xFFFF), bit a bit, para el
 * procesador del portátil (x86-64).
 *
 *   uint16_t crc16_x64(const uint8_t *datos, uint16_t longitud);
 *
 * Los parámetros llegan en rcx y rdx en Windows, y en rdi y rsi en Linux.
 */
        .text
        .globl  crc16_x64
crc16_x64:
#if defined(_WIN32)
        movq    %rcx, %r8               /* r8 = datos */
        movzwl  %dx, %r9d               /* r9 = longitud, de 16 bits */
#else
        movq    %rdi, %r8
        movzwl  %si, %r9d
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
