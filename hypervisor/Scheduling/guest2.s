# User Morpheus VM

.globl _start
    .code16
_start:
    xorw %ax, %ax
    
loop1:
#out %ax, $0x10
    dec %ax
    jmp loop1

