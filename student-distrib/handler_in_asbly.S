.text
.globl EXC_FINISHED
.globl ITR_SYS
.globl ITR_IRQ0,ITR_IRQ1,ITR_IRQ2,ITR_IRQ3,ITR_IRQ4,ITR_IRQ5,ITR_IRQ6,ITR_IRQ7,ITR_IRQ8,ITR_IRQ9,ITR_IRQA,ITR_IRQB,ITR_IRQC,ITR_IRQD,ITR_IRQE,ITR_IRQF
.globl ITR_DE,ITR_DB,ITR_NMI,ITR_BP,ITR_OF,ITR_BR,ITR_UD,ITR_NM,ITR_DF,ITR_CSO,ITR_TS,ITR_NP,ITR_SS,ITR_GP,ITR_PF,ITR_RES1,ITR_MF,ITR_AC,ITR_MC,ITR_XF,ITR_RES2
.globl irq_handler_jumptable, syscall_jumptable

irq_handler_jumptable:
    .rept 16
    .long EXC_FINISHED
    .endr

syscall_jumptable:
    .rept 10
    .long EXC_FINISHED
    .endr

# syscall
ITR_SYS:
    # check %eax
    cmpl  $1, %eax
    jl    ITR_SYS_FAIL
    cmpl  $8, %eax
    jg    ITR_SYS_FAIL
    # save REGs (except %eax)
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi

    # push args
    pushl %edx
    pushl %ecx
    pushl %ebx
    call  *syscall_jumptable(, %eax, 4)
    addl  $12, %esp   # 3*4

    # restore REGs
    popl  %edi
    popl  %esi
    popl  %ebp
    popl  %esp
    popl  %edx
    popl  %ecx
    popl  %ebx

    iret

ITR_SYS_FAIL:
    movl  $-1, %eax
    iret

# irq handler
ITR_IRQ0:
    pusha
    movl  $0, %eax
    jmp   ITR_IRQ

ITR_IRQ1:
    pusha
    movl  $1, %eax
    jmp   ITR_IRQ

ITR_IRQ2:
    pusha
    movl  $2, %eax
    jmp   ITR_IRQ

ITR_IRQ3:
    pusha
    movl  $3, %eax
    jmp   ITR_IRQ

ITR_IRQ4:
    pusha
    movl  $4, %eax
    jmp   ITR_IRQ

ITR_IRQ5:
    pusha
    movl  $5, %eax
    jmp   ITR_IRQ

ITR_IRQ6:
    pusha
    movl  $6, %eax
    jmp   ITR_IRQ

ITR_IRQ7:
    pusha
    movl  $7, %eax
    jmp   ITR_IRQ

ITR_IRQ8:
    pusha
    movl  $8, %eax
    jmp   ITR_IRQ

ITR_IRQ9:
    pusha
    movl  $9, %eax
    jmp   ITR_IRQ

ITR_IRQA:
    pusha
    movl  $10, %eax
    jmp   ITR_IRQ

ITR_IRQB:
    pusha
    movl  $11, %eax
    jmp   ITR_IRQ

ITR_IRQC:
    pusha
    movl  $12, %eax
    jmp   ITR_IRQ

ITR_IRQD:
    pusha
    movl  $13, %eax
    jmp   ITR_IRQ

ITR_IRQE:
    pusha
    movl  $14, %eax
    jmp   ITR_IRQ

ITR_IRQF:
    pusha
    movl  $15, %eax
    jmp   ITR_IRQ

ITR_IRQ:
    call *irq_handler_jumptable(, %eax, 4)
    popa
    iret

# halt when exception occurs
EXC_FINISHED:
    cli
    hlt
    jmp EXC_FINISHED
# https://wiki.osdev.org/Interrupt_Service_Routines

# interrupt handlers
ITR_DE:
    pusha
    call EXC_DE
    popa
    jmp EXC_FINISHED

ITR_DB:
    pusha
    cld
    call EXC_DB
    popa
    jmp EXC_FINISHED

ITR_NMI:
    pusha
    cld
    call EXC_NMI
    popa
    jmp EXC_FINISHED

ITR_BP:
    pusha
    cld
    call EXC_BP
    popa
    jmp EXC_FINISHED

ITR_OF:
    pusha
    cld
    call EXC_OF
    popa
    jmp EXC_FINISHED

ITR_BR:
    pusha
    cld
    call EXC_BR
    popa
    jmp EXC_FINISHED

ITR_UD:
    pusha
    cld
    call EXC_UD
    popa
    jmp EXC_FINISHED

ITR_NM:
    pusha
    cld
    call EXC_NM
    popa
    jmp EXC_FINISHED

ITR_DF:
    pusha
    cld
    call EXC_DF
    popa
    jmp EXC_FINISHED

ITR_CSO:
    pusha
    cld
    call EXC_CSO
    popa
    jmp EXC_FINISHED

ITR_TS:
    pusha
    cld
    call EXC_TS
    popa
    jmp EXC_FINISHED

ITR_NP:
    pusha
    cld
    call EXC_NP
    popa
    jmp EXC_FINISHED

ITR_SS:
    pusha
    cld
    call EXC_SS
    popa
    jmp EXC_FINISHED

ITR_GP:
    pusha
    cld
    call EXC_GP
    popa
    jmp EXC_FINISHED

ITR_PF:
    pusha
    cld
    call EXC_PF
    popa
    jmp EXC_FINISHED

ITR_RES1:
    pusha
    cld
    call EXC_RES1
    popa
    jmp EXC_FINISHED

ITR_MF:
    pusha
    cld
    call EXC_MF
    popa
    jmp EXC_FINISHED

ITR_AC:
    pusha
    cld
    call EXC_AC
    popa
    jmp EXC_FINISHED

ITR_MC:
    pusha
    cld
    call EXC_MC
    popa
    jmp EXC_FINISHED

ITR_XF:
    pusha
    cld
    call EXC_XF
    popa
    jmp EXC_FINISHED

ITR_RES2:
    pusha
    cld
    call EXC_RES2
    popa
    jmp EXC_FINISHED
