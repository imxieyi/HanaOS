#include <stddef.h>
#include <stdint.h>
#include "heap.hpp"
#include "isr.hpp"
#include "paging.hpp"
#include "hanastd.hpp"

uint32_t *current_directory;

static void page_fault(struct regs_t *regs)
{
    volatile uint32_t fault_addr;
    asm("mov %%cr2, %0" : "=r"(fault_addr));
    /*printf("=== Page Fault ===\n");
    printf("Faulting address: 0x%x\n", fault_addr);
    // dump the regs
    printf("EAX: 0x%x EBX: 0x%x\n", regs->eax, regs->ebx);
    printf("ECX: 0x%x EDX: 0x%x\n", regs->ecx, regs->edx);
    printf("ESP: 0x%x EBP: 0x%x\n", regs->esp, regs->ebp);
    printf("ESI: 0x%x EDI: 0x%x\n", regs->esi, regs->edi);
    printf("EIP: 0x%x\n", regs->eip);
    panic("Page fault!\n");*/
}

 void paging_switch_directory(uint32_t *dir)
{
    current_directory = dir;
    do_paging_enable((uint32_t)current_directory);
}

static uint32_t *identity_map_table(uint32_t start, int flags)
{
    /* make a page table */
    uint32_t *table = kmalloc_a(0x1000);
    /* identity map 4MB */
    for(uint32_t i = start; i < start + 1024; ++i)
    {
        table[i - start] = (i * PAGE_SIZE) | flags;
    }
    return table;
}

void paging_init(void)
{
    uint32_t *kernel_directory = kmalloc_a(0x1000);
    hanastd::memset(kernel_directory, 0, 0x1000);
    /* blank the kernel directory */
    for(int i = 0; i < 1024; ++i)
    {
        kernel_directory[i] = PAGE_RW;
    }

    /* identity map all 4GB (and allocate all page tables) */
    for(int i = 0; i < 1024; ++i)
        kernel_directory[i] = (uint32_t)identity_map_table(i * 1024, PAGE_PRESENT | PAGE_RW) |
            PAGE_PRESENT | PAGE_RW;

    register_interrupt_handler(14, page_fault);

    paging_switch_directory(kernel_directory);
}
