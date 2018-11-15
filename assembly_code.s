	.globl main
main:
	enter $(-8), $0
	movq $12 (%rbp)
	movq -64(%rbp) (%rbp)
	ret

