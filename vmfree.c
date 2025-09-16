#include "vm.h"
#include "vmlib.h"
#include <stdio.h>

/**
 * The vmfree() function frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to vmmalloc().
 * Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs.
 * If ptr is NULL, no operation is performed.
 */
void vmfree(void *ptr)
{



    if (!ptr) return;

    struct block_header* block2 = (struct block_header*)((char*)ptr - sizeof(struct block_header));
    block2->size_status &= ~VM_BUSY;

    struct block_header *nexxt2 = (struct block_header *)((char *)block2 + (block2->size_status & VM_BLKSZMASK));

    if ((nexxt2->size_status & VM_BUSY) == 0 && nexxt2->size_status != VM_ENDMARK) {
        block2->size_status += nexxt2->size_status & VM_BLKSZMASK;
    }

    if ((block2->size_status & VM_PREVBUSY) == 0) {
        struct block_footer *prevv2 = (struct block_footer *)((char *)block2 - sizeof(struct block_footer));

        if (prevv2->size > 0) { 
            struct block_header *prevvblock = (struct block_header *)((char *)block2 - prevv2->size);
            prevvblock->size_status += block2->size_status & VM_BLKSZMASK;
            block2 = prevvblock; 
        }
    }

    struct block_footer *footer = (struct block_footer *)((char *)block2 + (block2->size_status & VM_BLKSZMASK) - sizeof(struct block_footer));
    footer->size = block2->size_status & VM_BLKSZMASK;

}
