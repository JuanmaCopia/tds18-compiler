	.globl main
main:
	enter	$(24), $0
	push 	$5 
	call 	myfunc 
	movq 	%rax, -24(%rbp) 
	pop  	%rcx 
	movq 	-24(%rbp), -8(%rbp) 
	leave
	ret
myfunc:
	enter	$(8), $0
	movq 	$1, %rax 
	leave
	ret
