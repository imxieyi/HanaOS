#pragma once
extern "C" void gdt_flush(uint32_t);

typedef struct {
    uint16_t limit_low;    // lower 16 bits of limit
    uint16_t base_low;     // lower 16 bits of base
    uint8_t  base_middle;  // next 8 bits of base
    uint8_t  access;       // access flags
    uint8_t  granularity;  // granularity flags
    uint8_t  base_high;    // upper 8 bits of base
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;        // upper 16 bits of all selector limits
    uintptr_t base;         // address of the first gdt_entry_t
} __attribute__((packed)) gdt_t;

void set_entry(uint8_t num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);
void gdt_init(void);
