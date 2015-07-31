    .text
    .align    2

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