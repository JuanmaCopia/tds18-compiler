	.globl main
main:
	enter $(32), $0
	movq -24(%rbp) %rax
	movq -32(%rbp) %rdx
	add %rdx %rax
	movq %rax -16(%rbp)
	movq -16(%rbp) -8(%rbp)
