	.globl main
main:
	enter	$(64), $0
	movq 	$2, -32(%rbp) 
	movq 	-32(%rbp), %r10
	movq 	%r10, -16(%rbp)
	movq 	$0, -40(%rbp) 
	movq 	-40(%rbp), %r10
	movq 	%r10, -24(%rbp)
	movq 	$1, -56(%rbp) 
	movq 	$0, -64(%rbp) 
	movq 	-56(%rbp), %rax 
	movq 	-64(%rbp), %rdx 
	add  	%rdx, %rax 
	cmp  	$0, %rax 
	je  	asmlabel0 
	jmp  	asmlabel1 
asmlabel0:
	movq 	$0, -48(%rbp) 
	jmp  	asmlabel2 
asmlabel1:
	movq 	$1, -48(%rbp) 
asmlabel2:
	movq 	-48(%rbp), %r10
	movq 	%r10, -8(%rbp)
	leave
	ret
myfunc:
	enter	$(8), $0
	movq 	$1, %rax 
	leave
	ret
