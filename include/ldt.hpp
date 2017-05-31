#ifndef _IDT_HPP
#define _IDT_HPP

extern "C" void ldt_flush(uint32_t);

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

#define PIC_MASTER_CMD   0x20
#define PIC_MASTER_DATA  0x21
#define PIC_SLAVE_CMD    0xa0
#define PIC_SLAVE_DATA   0xa1

typedef struct {
    uint16_t base_low;   // lower 16 bits of address to jump to if this int fires
    uint16_t selector;   // kernel segment selector
    uint8_t  reserved;   // always zero
    uint8_t  flags;
    uint16_t base_high;  // upper 16 bits of address to jump to if this int fires
} __attribute__((packed)) ldt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;   // address of first element in gdt_t
} __attribute__((packed)) ldt_t;

void ldt_init(void);
#endif
