[bits 32]
	global io_hlt,io_cli,io_sti,io_stihlt,io_in8,io_in16,io_in32
	global io_out8,io_out16,io_out32,io_load_eflags,io_store_eflags
	global load_gdtr,load_idtr,load_cr0,store_cr0,memtest
	global switchTask
section .text

io_hlt:
	hlt
	ret

io_cli:
	cli
	ret

io_sti:
	sti
	ret

io_stihlt:
	sti
	hlt
	ret

io_in8:
	mov		edx,[esp+4]
	mov		eax,0
	in		al,dx
	ret

io_in16:
	mov		edx,[esp+4]
	mov		eax,0
	in		ax,dx
	ret

io_in32:
	mov		edx,[esp+4]
	in		eax,dx
	ret

io_out8:
	mov		edx,[esp+4]
	mov		al,[esp+8]
	out		dx,al
	ret

io_out16:
	mov		edx,[esp+4]
	mov		eax,[esp+8]
	out		dx,ax
	ret

io_out32:
	mov		edx,[esp+4]
	mov		eax,[esp+8]
	out		dx,eax
	ret

io_load_eflags:
	pushfd
	pop		eax
	ret

io_store_eflags:
	mov		eax,[esp+4]
	push	eax
	popfd
	ret

load_gdtr:
	mov		ax,[esp+4]
	mov		[esp+6],ax
	lgdt	[esp+6]
	ret

load_idtr:
	mov		ax,[esp+4]
	mov		[esp+6],ax
	lidt	[esp+6]
	ret

load_cr0:
	mov		eax,cr0
	ret

store_cr0:
	mov		eax,[esp+4]
	mov		cr0,eax
	ret

; 改进：vmware要求内存必须是4MB的倍数，所以直接以4MB为单位检查内存
memtest:
	push	edi
	push	esi
	push	ebx
	mov		esi,0x66996699
	mov		edi,0x99669966
	mov		eax,[esp+16]
mts_loop:
	mov		ebx,eax
	add		ebx,0x3ffffc
	mov		edx,[ebx]
	mov		[ebx],esi
	xor		dword[ebx],0xffffffff
	cmp		edi,[ebx]
	jne		mts_fin
	xor		dword[ebx],0xffffffff
	cmp		esi,[ebx]
	jne		mts_fin
	mov		[ebx],edx
	add		eax,0x400000
	cmp		eax,[esp+20]
	jbe		mts_loop
	pop		ebx
	pop		esi
	pop		edi
	ret
mts_fin:
	mov		[ebx],edx
	pop		ebx
	pop		esi
	pop		edi
	ret

switchTask:
	pusha
	pushf
	mov		eax,cr3
	push	eax
	mov		eax,[esp+44]
	mov		[eax+4],ebx
	mov		[eax+8],ecx
	mov		[eax+12],edx
	mov		[eax+16],esi
	mov		[eax+20],edi
	mov		ebx,[esp+36]
	mov		ecx,[esp+40]
	mov		edx,[esp+20]
	add		edx,4
	mov		esi,[esp+16]
	mov		edi,[esp+4]
	mov		[eax],ebx
	mov		[eax+24],edx
	mov		[eax+28],esi
	mov		[eax+32],ecx
	mov		[eax+36],edi
	pop		ebx
	mov		[eax+40],ebx
	push	ebx
	mov		eax,[esp+48]
	mov		ebx,[eax+4]
	mov		ecx,[eax+8]
	mov		edx,[eax+12]
	mov		esi,[eax+16]
	mov		edi,[eax+20]
	mov		ebp,[eax+28]
	push	eax
	mov		eax,[eax+36]
	push	eax
	popf
	pop		eax
	mov		esp,[eax+24]
	push	eax
	mov		eax,[eax+40]
	mov		cr3,eax
	pop		eax
	push	eax
	mov		eax,[eax+32]
	xchg	eax,[esp]
	mov		eax,[eax]
	sti
	ret
