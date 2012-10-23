	.file	"stride.c"
	.text
	.p2align 4,,15
.globl entryPoint
	.type	entryPoint, @function
entryPoint:
.LFB22:
	.cfi_startproc
    sub $32, %rsp
    #Store r10~r13
    mov %r10, 0(%rsp)
    mov %r11, 8(%rsp)
    mov %r12, 16(%rsp)
    mov %r13, 24(%rsp)
    #Load r10~r12
    mov 40(%rsp), %r10
    mov 48(%rsp), %r11
    mov 56(%rsp), %r12
    #Reset eax, r13
	xorl	%eax, %eax
    xor %r13, %r13
    #To be safe, remove a bit...
    sub     $4, %rdi
    testq	%rdi, %rdi
    je	.L3
    .p2align 4,,10
    .p2align 3



.L6:

	movsd 8(%rsi), %xmm0
	movsd 1032(%rsi), %xmm1
	movsd 1024(%rsi), %xmm2
	movsd 1040(%rsi), %xmm3
	movsd 16(%rsi), %xmm4
	movsd 0(%rsi), %xmm5
	movsd 24(%rsi), %xmm6
	movsd 2056(%rsi), %xmm7
	movsd 1048(%rsi), %xmm8
	movsd 1040(%rsi), %xmm9
	movsd 1056(%rsi), %xmm10
	movsd 2064(%rsi), %xmm11
	movsd 32(%rsi), %xmm12
	movsd 2048(%rsi), %xmm13
	movsd 1056(%rsi), %xmm14
	movsd 16(%rsi), %xmm15
	movsd 32(%rsi), %xmm0
	movsd 2064(%rsi), %xmm1
	movsd 2080(%rsi), %xmm2
	addss %xmm0, %xmm1
	addss %xmm2, %xmm3
	addss %xmm4, %xmm5
	addss %xmm6, %xmm7
	addss %xmm8, %xmm9
	addss %xmm10, %xmm11
	addss %xmm12, %xmm13
	addss %xmm14, %xmm15
	addss %xmm0, %xmm1
	addss %xmm2, %xmm3
	addss %xmm4, %xmm5
	addss %xmm6, %xmm7
	addss %xmm8, %xmm9
	mulss %xmm10, %xmm11
	mulss %xmm12, %xmm13
	mulss %xmm14, %xmm15
	mulss %xmm0, %xmm1
	mulss %xmm2, %xmm3
	mulss %xmm4, %xmm5
	mulss %xmm6, %xmm7
	mulss %xmm8, %xmm9
	mulss %xmm10, %xmm11
	movsd %xmm0, 1048(%rdx)
	movsd 2072(%rcx), %xmm0
	movsd 2080(%rcx), %xmm1
	addss %xmm0, %xmm1
	addss %xmm0, %xmm1
	addss %xmm0, %xmm1
	movsd %xmm0, 1032(%r8)
	#Did not Unroll from 1 to 1
	#Unrolled factor 1
	#Induction variables
	##Induction variable: 1 , 1 , 1
	add $1, %eax
	##Induction variable: 1 , 4 , 1
	add $4, %rsi
	##Induction variable: 1 , 4 , 1
	add $4, %rdx
	##Induction variable: 1 , 4 , 1
	add $4, %rcx
	##Induction variable: 1 , 4 , 1
	add $4, %r8
	sub $1, %rdi
	jge .L6

.L3:
    mov 0(%rsp), %r10
    mov 8(%rsp), %r11
    mov 16(%rsp), %r12
    mov 24(%rsp), %r13
    add $32, %rsp

	ret
	.cfi_endproc
.LFE22:
	.size	entryPoint, .-entryPoint
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits

