        global do_paging_enable
do_paging_enable:
        mov eax,[esp+4] ; loads page directory address
        mov cr3,eax
        mov eax,cr0
        or eax,0x80000000 ; set PG bit
        sti
        mov cr0,eax
        ret
