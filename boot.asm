global start, KERNEL_VMA_OFFSET, KERNEl_PMA_OFFSET
extern kernel_main

MODULEALIGN equ 1<<0
MEMINFO     equ 1<<1
GRAPHICS	equ 1<<2
FLAGS       equ MODULEALIGN | MEMINFO | GRAPHICS
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)
; multiboot显示设置
G_MODE		equ 0
G_WIDTH		equ 1024
G_HEIGHT	equ 768
G_DEPTH		equ 32

section .multiboot
bits 32
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    resd 5
    dd G_MODE
    dd G_WIDTH
    dd G_HEIGHT
    dd G_DEPTH

section .bss
kernel_stack_bottom:
	resb 32768
kernel_stack_top:
    
section .text
bits 32
start:
	mov esp,kernel_stack_top
	push 0
	popf
	push eax
	push ebx
;	add esp,8
    call kernel_main
