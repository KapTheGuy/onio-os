#include "types.h"
// #include "defs.h"
// #include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"

// static void startothers(void);
// static void mpmain(void)  __attribute__((noreturn));
// extern pde_t *kpgdir;
extern char end[]; // first address after kernel loaded from ELF file

// Bootstrap processor starts running C code here.
// Allocate a real stack and switch to it, first
// doing some setup required for memory allocator to work.

enum vga_color 
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline unsigned char vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline unsigned short vga_entry(unsigned char uc, unsigned char color) 
{
	return (unsigned short) uc | (unsigned short) color << 8;
}

unsigned short *buffer;

void prints(const char* str)
{
    int index = 0;
	unsigned char color_term = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    while(str[index] != '\0')
    {
        buffer[index] = vga_entry(str[index], color_term);
        index++;
    }
}

int
main(void)
{
    buffer = (unsigned short*) 0xB8000;

    unsigned char color_term = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (int y = 0; y < 25; y++) 
    {
		for (int x = 0; x < 80; x++) 
        {
			const int index = y * 80 + x;
			buffer[index] = vga_entry(' ', color_term);
		}
	}

    prints("Hello World From The Kernel! :D");

    while(1);
}

// Other CPUs jump here from entryother.S.
// static void
// mpenter(void)
// {
//   // switchkvm();
//   // seginit();
//   // lapicinit();
//   // mpmain();
// }

// Common CPU setup code.
// static void
// mpmain(void)
// {
//   cprintf("cpu%d: starting %d\n", cpuid(), cpuid());
//   idtinit();       // load idt register
//   xchg(&(mycpu()->started), 1); // tell startothers() we're up
//   scheduler();     // start running processes
// }

pde_t entrypgdir[];  // For entry.S

// // Start the non-boot (AP) processors.
// static void
// startothers(void)
// {
//   extern uchar _binary_entryother_start[], _binary_entryother_size[];
//   uchar *code;
//   struct cpu *c;
//   char *stack;

//   // Write entry code to unused memory at 0x7000.
//   // The linker has placed the image of entryother.S in
//   // _binary_entryother_start.
//   code = P2V(0x7000);
//   memmove(code, _binary_entryother_start, (uint)_binary_entryother_size);

//   for(c = cpus; c < cpus+ncpu; c++){
//     if(c == mycpu())  // We've started already.
//       continue;

//     // Tell entryother.S what stack to use, where to enter, and what
//     // pgdir to use. We cannot use kpgdir yet, because the AP processor
//     // is running in low  memory, so we use entrypgdir for the APs too.
//     stack = kalloc();
//     *(void**)(code-4) = stack + KSTACKSIZE;
//     *(void(**)(void))(code-8) = mpenter;
//     *(int**)(code-12) = (void *) V2P(entrypgdir);

//     lapicstartap(c->apicid, V2P(code));

//     // wait for cpu to finish mpmain()
//     while(c->started == 0)
//       ;
//   }
// }

// // The boot page table used in entry.S and entryother.S.
// // Page directories (and page tables) must start on page boundaries,
// // hence the __aligned__ attribute.
// // PTE_PS in a page directory entry enables 4Mbyte pages.

__attribute__((__aligned__(PGSIZE)))
pde_t entrypgdir[NPDENTRIES] = {
  // Map VA's [0, 4MB) to PA's [0, 4MB)
  [0] = (0) | PTE_P | PTE_W | PTE_PS,
  // Map VA's [KERNBASE, KERNBASE+4MB) to PA's [0, 4MB)
  [KERNBASE>>PDXSHIFT] = (0) | PTE_P | PTE_W | PTE_PS,
};

// //PAGEBREAK!
// // Blank page.
// //PAGEBREAK!
// // Blank page.
// //PAGEBREAK!
// // Blank page.

