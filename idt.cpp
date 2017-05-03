/*
 * Copyright (c) 2013 Marco Hinz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the author nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include "idt.hpp"
#include "asmfunc.hpp"
#include "hanastd.hpp"

idt_t       idt;
idt_entry_t idt_entries[0x100];

static void
set_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_low  = (base & 0xffff);
    idt_entries[num].base_high = ((uint16_t)(base >> 16) & 0xffff);
    idt_entries[num].selector  = sel;
    idt_entries[num].reserved  = 0;
    idt_entries[num].flags     = flags;  // flags | 0x60 for user-mode
}

void
idt_init(void)
{
   uint32_t size = sizeof (idt_entry_t) * 256;

    idt.limit = (uint16_t)((uint16_t)size - 1);
    idt.base  = (uint32_t)&idt_entries;

    hanastd::memset(&idt_entries, 0, size);

    /*
     * remap IRQs
     */

    // start initialization sequence (cascade mode)
    io_out8(PIC_MASTER_CMD,  0x11);
    io_out8(PIC_SLAVE_CMD,   0x11);

    io_out8(PIC_MASTER_DATA, 0x20);
    io_out8(PIC_SLAVE_DATA,  0x28);
    io_out8(PIC_MASTER_DATA, 0x04);
    io_out8(PIC_SLAVE_DATA,  0x02);
    io_out8(PIC_MASTER_DATA, 0x01);
    io_out8(PIC_SLAVE_DATA,  0x01);
    io_out8(PIC_MASTER_DATA, 0x0);
    io_out8(PIC_SLAVE_DATA,  0x0);

    /*
     * set IDT entries
     */

    // CPU interrupts
    set_entry(0,  (uint32_t)isr0,  0x08, 0x8e);
    set_entry(1,  (uint32_t)isr1,  0x08, 0x8e);
    set_entry(2,  (uint32_t)isr2,  0x08, 0x8e);
    set_entry(3,  (uint32_t)isr3,  0x08, 0x8e);
    set_entry(4,  (uint32_t)isr4,  0x08, 0x8e);
    set_entry(5,  (uint32_t)isr5,  0x08, 0x8e);
    set_entry(6,  (uint32_t)isr6,  0x08, 0x8e);
    set_entry(7,  (uint32_t)isr7,  0x08, 0x8e);
    set_entry(8,  (uint32_t)isr8,  0x08, 0x8e);
    set_entry(9,  (uint32_t)isr9,  0x08, 0x8e);
    set_entry(10, (uint32_t)isr10, 0x08, 0x8e);
    set_entry(11, (uint32_t)isr11, 0x08, 0x8e);
    set_entry(12, (uint32_t)isr12, 0x08, 0x8e);
    set_entry(13, (uint32_t)isr13, 0x08, 0x8e);
    set_entry(14, (uint32_t)isr14, 0x08, 0x8e);
    set_entry(15, (uint32_t)isr15, 0x08, 0x8e);
    set_entry(16, (uint32_t)isr16, 0x08, 0x8e);
    set_entry(17, (uint32_t)isr17, 0x08, 0x8e);
    set_entry(18, (uint32_t)isr18, 0x08, 0x8e);
    set_entry(19, (uint32_t)isr19, 0x08, 0x8e);
    set_entry(20, (uint32_t)isr20, 0x08, 0x8e);
    set_entry(21, (uint32_t)isr21, 0x08, 0x8e);
    set_entry(22, (uint32_t)isr22, 0x08, 0x8e);
    set_entry(23, (uint32_t)isr23, 0x08, 0x8e);
    set_entry(24, (uint32_t)isr24, 0x08, 0x8e);
    set_entry(25, (uint32_t)isr25, 0x08, 0x8e);
    set_entry(26, (uint32_t)isr26, 0x08, 0x8e);
    set_entry(27, (uint32_t)isr27, 0x08, 0x8e);
    set_entry(28, (uint32_t)isr28, 0x08, 0x8e);
    set_entry(29, (uint32_t)isr29, 0x08, 0x8e);
    set_entry(30, (uint32_t)isr30, 0x08, 0x8e);
    set_entry(31, (uint32_t)isr31, 0x08, 0x8e);
    // PIC interrupts
    set_entry(32, (uint32_t)irq0,  0x08, 0x8e);
    set_entry(33, (uint32_t)irq1,  0x08, 0x8e);
    set_entry(34, (uint32_t)irq2,  0x08, 0x8e);
    set_entry(35, (uint32_t)irq3,  0x08, 0x8e);
    set_entry(36, (uint32_t)irq4,  0x08, 0x8e);
    set_entry(37, (uint32_t)irq5,  0x08, 0x8e);
    set_entry(38, (uint32_t)irq6,  0x08, 0x8e);
    set_entry(39, (uint32_t)irq7,  0x08, 0x8e);
    set_entry(40, (uint32_t)irq8,  0x08, 0x8e);
    set_entry(41, (uint32_t)irq9,  0x08, 0x8e);
    set_entry(42, (uint32_t)irq10, 0x08, 0x8e);
    set_entry(43, (uint32_t)irq11, 0x08, 0x8e);
    set_entry(44, (uint32_t)irq12, 0x08, 0x8e);
    set_entry(45, (uint32_t)irq13, 0x08, 0x8e);
    set_entry(46, (uint32_t)irq14, 0x08, 0x8e);
    set_entry(47, (uint32_t)irq15, 0x08, 0x8e);

    idt_flush((uint32_t)&idt);

//    __asm__ volatile ("sti");
}
