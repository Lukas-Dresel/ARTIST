    .text
    .align    2
    .global     hook_init
    .type       hook_init, %function

    @Register Struct:
    @   r0,     r1,     r2,     r3,     16 Byte
    @   r4,     r5,     r6,     r7,     16 Byte
    @   r8,     r9,     r10,    r11,    16 Byte
    @   r12,    r13,    r14,    r15     16 Byte
    @ ===========================================
    @                                   64 Byte

hook_init:
    push    {r4}
    sub	    sp,     #64	    @ 0x40
    str     r0,     [sp, #0 ]
    str     r1,     [sp, #4 ]
    str     r2,     [sp, #8 ]
    str     r3,     [sp, #12]
    str     r4,     [sp, #16]
    str     r5,     [sp, #20]
    str     r6,     [sp, #24]
    str     r7,     [sp, #28]
    str     r8,     [sp, #32]
    str     r9,     [sp, #36]
    str     r10,    [sp, #40]
    str     r11,    [sp, #44]
    str     r12,    [sp, #48]
    str     r13,    [sp, #52]
    str     r14,    [sp, #56]
    str     r15,    [sp, #60]





    .arm
    .global hook_stub_arm
    .type hook_stub_arm, %function


hook_stub_arm:
    ldr pc, [pc, #4]
    nop
    nop
    .word   0x12345678
    .size    hook_stub_arm, .-hook_stub_arm



    .thumb
    .global hook_stub_thumb
    .type hook_stub_thumb, %function

hook_stub_thumb:
    sub     sp, sp, #8
    str     r0, [sp]

    add     r0, pc, #4
    ldr     r0, [r0, #0]

    str     r0, [sp, #4]
    pop     {r0, pc}

    @The address to jump to.
    .word 0x12345678

    .size    hook_stub_thumb, .-hook_stub_thumb







    .arm
    .global     armFunction
    .type    armFunction, %function
    armFunction:
        @ Multiply by 10. Input value and return value in r0
        stmfd    sp!, {fp,ip,lr}
        mov    r3, r0, asl #3
        add    r0, r3, r0, asl #1
        ldmfd    sp!, {fp,ip,lr}
        bx    lr
        .size    armFunction, .-armFunction







