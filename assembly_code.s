	.globl main
main:
	enter	$(80), $0
	movq 	$1, -8(%rbp) 
	movq 	-8(%rbp), %rdi 
	movq 	$2, -16(%rbp) 
	movq 	-16(%rbp), %rsi 
	movq 	$3, -24(%rbp) 
	movq 	-24(%rbp), %rdx 
	movq 	$4, -32(%rbp) 
	movq 	-32(%rbp), %rcx 
	movq 	$5, -40(%rbp) 
	movq 	-40(%rbp), %r8 
	movq 	$6, -48(%rbp) 
	movq 	-48(%rbp), %r9 
	push 	$7 
	push 	$8 
	push 	$9 
	call 	externfun 
	movq 	%rax, -80(%rbp) 
	pop  	%rcx 
	pop  	%rcx 
	pop  	%rcx 
	leave
	ret
