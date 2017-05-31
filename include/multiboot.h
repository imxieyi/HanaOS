#pragma once
#ifdef __cplusplus
extern "C"{
#endif
/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/* The Multiboot header.  */
typedef struct multiboot_header
{
  unsigned long magic;
  unsigned long flags;
  unsigned long checksum;

  unsigned long header_addr;
  unsigned long load_addr;
  unsigned long load_end_addr;
  unsigned long bss_end_addr;
  unsigned long entry_addr;

  /* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
  unsigned long mode_type;
  unsigned long width;
  unsigned long height;
  unsigned long depth;
} multiboot_header_t;

/* The symbol table for a.out.  */
typedef struct aout_symbol_table
{
  unsigned long tabsize;
  unsigned long strsize;
  unsigned long addr;
  unsigned long reserved;
} aout_symbol_table_t;

/* The section header table for ELF.  */
typedef struct elf_section_header_table
{
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information.  */
typedef struct multiboot_info
{
  unsigned long flags;
  unsigned long mem_lower;
  unsigned long mem_upper;
  unsigned long boot_device;
  unsigned long cmdline;
  unsigned long mods_count;
  unsigned long mods_addr;
  union
  {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  unsigned long mmap_length;
  unsigned long mmap_addr;

  /* Drive Info buffer */
  unsigned int drives_length;
  unsigned int drives_addr;

  /* ROM configuration table */
  unsigned int config_table;

  /* Boot Loader Name */
  unsigned int boot_loader_name;

  /* APM table */
  unsigned int apm_table;

  /* Video */
  unsigned int vbe_control_info;
  unsigned int vbe_mode_info;
  unsigned short vbe_mode;
  unsigned short vbe_interface_seg;
  unsigned short vbe_interface_off;
  unsigned short vbe_interface_len;
} multiboot_info_t;

/* The module structure.  */
typedef struct module
{
  unsigned long mod_start;
  unsigned long mod_end;
  unsigned long string;
  unsigned long reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct memory_map
{
  unsigned long size;
  unsigned long base_addr_low;
  unsigned long base_addr_high;
  unsigned long length_low;
  unsigned long length_high;
  unsigned long type;
} memory_map_t;

typedef struct vbe_control_info {
  char VbeSignature[4];                   // == "VESA"
  unsigned short VbeVersion;              // == 0x0300 for VBE 3.0
  unsigned short OemStringPtr[2];         // isa vbeFarPtr
  unsigned char Capabilities[4];
  unsigned short VideoModePtr[2];         // isa vbeFarPtr
  unsigned short TotalMemory;             // as # of 64KB blocks
} __attribute__((packed)) vbe_control_info_t;

typedef struct vbe_mode_info {
  unsigned short attributes;
  unsigned char winA,winB;
  unsigned short granularity;
  unsigned short winsize;
  unsigned short segmentA, segmentB;
  unsigned long winFuncPtr; // ptr to INT 0x10 Function 0x4F05
  unsigned short pitch; // bytes per scanline

  unsigned short Xres, Yres;
  unsigned char Wchar, Ychar, planes, bpp, banks;
  unsigned char memory_model, bank_size, image_pages;
  unsigned char reserved0;

  unsigned char red_mask_size, red_position;
  unsigned char green_mask_size, green_position;
  unsigned char blue_mask_size, blue_position;
  unsigned char rsv_mask, rsv_position;
  unsigned char directcolor_attributes;

  unsigned long physbase;  // your LFB (Linear Framebuffer) address ;)
  unsigned long reserved1;
  unsigned short reserved2;
} __attribute__((packed)) vbe_mode_info_t;

typedef enum
{
  memPL = 3, /* Planar memory model */
  memPK = 4, /* Packed pixel memory model */
  memRGB = 6, /* Direct color RGB memory model */
  memYUV = 7, /* Direct color YUV memory model */
} memModels;

#ifdef __cplusplus
}
#endif
