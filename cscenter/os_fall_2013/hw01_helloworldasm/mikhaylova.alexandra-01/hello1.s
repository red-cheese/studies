.text
		.global	main

main:
		movl	$4, %eax
		movl	$1, %ebx	# stdout
		movl	$msg, %ecx
		movl	$len, %edx
		int	$0x80

		movl	$1, %eax
		movl	$0, %ebx
		int $0x80


.data

msg:
		.string "Hello world!\n"	# null-terminated string
		len = . - msg
