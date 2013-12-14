.data

msg:
        .ascii  "Hello, World!\n"
len = . - msg

.text

.global hello
hello:
        /* syscall write(int fd, const void *buf, size_t count) */
        mov     %r0, $1
        ldr     %r1, =msg
        ldr     %r2, =len
        mov     %r7, $4
        swi     $0

        /* syscall exit(int status) */
        mov     %r0, $0
        mov     %r7, $1
        swi     $0

