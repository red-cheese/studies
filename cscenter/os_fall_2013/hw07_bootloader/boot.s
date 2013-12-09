.code16
.text

.globl _start
_start:
        jmp boot
        data : .asciz "Hello, World!\r\n"

print:
        lodsb
        orb  %al, %al
        jz  print_done
        movb  $0x0e, %ah
        int $0x10
        jmp print

print_done:
        ret

reboot:
        call key_pressed
        .byte 0xea
        .word 0x0000
        .word 0xffff

key_pressed:
        movb  $0x0, %ah
        int $0x16
        ret

boot:
        movw $0x07c0, %ax
        movw %ax, %ds

        movw $0x02, %si
        call print
        call reboot

	. = _start + 510
	.byte 0x55
	.byte 0xaa
