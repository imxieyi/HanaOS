global gdt_flush
global ldt_flush

gdt_flush:
    mov     eax, [esp+4]  ; get pointer to GDT
    lgdt    [eax]         ; load new GDT

    mov     ax, 0x10      ; third GTD entry (kernel data segment)
    mov     ds, ax        ; load all kernel data segment selectors
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    jmp     0x08:.flush   ; offset to our kernel code segment

.flush:
    ret

ldt_flush:
    mov     eax, [esp+4]  ; get pointer to IDT
    lldt    [eax]         ; load new IDT
    ret
