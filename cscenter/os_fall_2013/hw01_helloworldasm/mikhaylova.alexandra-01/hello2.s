.text
		.global	main

main:
		pushl	$msg
		call	puts
		add	$4, %esp

		movl	$1, %eax
		movl	$0, %ebx
		int $0x80


.data

msg:
		.string "Hello world!"	# null-terminated string
		len = . - msg
