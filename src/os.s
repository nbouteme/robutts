	.file	"os.c"
	.text
	.globl	nano_diff
	.type	nano_diff, @function
nano_diff:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, %rax
	movq	%rsi, %r8
	movq	%rax, %rsi
	movq	%rdx, %rdi
	movq	%r8, %rdi
	movq	%rsi, -32(%rbp)
	movq	%rdi, -24(%rbp)
	movq	%rdx, -48(%rbp)
	movq	%rcx, -40(%rbp)
	movq	$0, -8(%rbp)
	movq	-32(%rbp), %rax
	imulq	$1000000000, %rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	addq	%rax, -8(%rbp)
	movq	-48(%rbp), %rax
	imulq	$1000000000, %rax, %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	subq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	nano_diff, .-nano_diff
	.section	.rodata
.LC0:
	.string	"sh"
.LC1:
	.string	"-c"
	.text
	.globl	make_linked_process
	.type	make_linked_process, @function
make_linked_process:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-64(%rbp), %rax
	movq	%rax, %rdi
	call	pipe@PLT
	leaq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	pipe@PLT
	call	fork@PLT
	movl	%eax, %edx
	movq	-72(%rbp), %rax
	movl	%edx, (%rax)
	movq	-72(%rbp), %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	jne	.L4
	movl	-60(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	-56(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	-64(%rbp), %eax
	movl	$0, %esi
	movl	%eax, %edi
	call	dup2@PLT
	movl	-52(%rbp), %eax
	movl	$1, %esi
	movl	%eax, %edi
	call	dup2@PLT
	leaq	.LC0(%rip), %rax
	movq	%rax, -48(%rbp)
	leaq	.LC1(%rip), %rax
	movq	%rax, -40(%rbp)
	movq	-80(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	$0, -24(%rbp)
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rdi
	call	execvp@PLT
	movl	$0, %eax
	jmp	.L6
.L4:
	movl	-64(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	-52(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	-60(%rbp), %edx
	movq	-72(%rbp), %rax
	movl	%edx, 4(%rax)
	movl	-56(%rbp), %edx
	movq	-72(%rbp), %rax
	movl	%edx, 8(%rax)
	movl	$1, %eax
.L6:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L7
	call	__stack_chk_fail@PLT
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	make_linked_process, .-make_linked_process
	.ident	"GCC: (GNU) 7.2.0"
	.section	.note.GNU-stack,"",@progbits
