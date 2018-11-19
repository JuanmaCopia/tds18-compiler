	.globl main
main:
	enter	$(48), $0
	movq 	$1, -32(%rbp) 
	movq 	-32(%rbp), -16(%rbp) 
	movq 	$0, -40(%rbp) 
	movq 	-40(%rbp), -24(%rbp) 
	movq 	-16(%rbp), %rax 
	movq 	-24(%rbp), %rdx 
	add  	%rdx, %rax 
	cmp  	$0, %rax 
	je  	.false_label 
	jmp  	.true_label 
.false_label:
	movq 	$0, -48(%rbp) 
.true_label:
	movq 	$1, -48(%rbp) 
	movq 	-48(%rbp), -8(%rbp) 
