.text
.align    2

.global breakpoint_arm
.global breakpoint_thumb

.arm
breakpoint_arm:
    bkpt
    .size    breakpoint_arm, .-breakpoint_arm

.thumb
breakpoint_thumb:
    bkpt
    .size    breakpoint_thumb, .-breakpoint_thumb