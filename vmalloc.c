#include "vm.h"
#include "vmlib.h"

void *vmalloc(size_t size)
{
    
    if (size == 0) {
        return NULL;
    }

    size += sizeof(struct block_header);
    size = ROUND_UP(size, BLKALIGN);

    struct block_header *fit = NULL;
    struct block_header *curr = heapstart;
    size_t best_fit_leftover = 0;

    while (curr->size_status != VM_ENDMARK) 
    {
        size_t block_size = curr->size_status & VM_BLKSZMASK;
        int busy = curr->size_status & VM_BUSY;
        size_t leftover = block_size - size;

        if (!busy && block_size >= size) 
        {
            if (fit == NULL || leftover < best_fit_leftover) 
            {
                fit = curr;
                best_fit_leftover = leftover;
            }
            if (leftover == 0) 
            {
                break;
            }
        }

        curr = (struct block_header *)((char *)curr + block_size);
    }

    if (!fit) 
    {
        return NULL;
    }

    size_t bsize = fit->size_status & VM_BLKSZMASK;
    if (bsize - size >= sizeof(struct block_header) + sizeof(struct block_footer)) 
    {
        struct block_header *split_block = (struct block_header *)((char *)fit + size);
        split_block->size_status = (bsize - size) | VM_PREVBUSY;
        
        struct block_footer *split_footer = (struct block_footer *)((char *)split_block + (split_block->size_status & VM_BLKSZMASK) - sizeof(struct block_footer));
        split_footer->size = split_block->size_status & VM_BLKSZMASK;
    }

    fit->size_status = size | VM_BUSY | (fit->size_status & VM_PREVBUSY);

    struct block_header *next_block = (struct block_header *)((char *)fit + (fit->size_status & VM_BLKSZMASK));
    if (next_block->size_status != VM_ENDMARK) 
    {
        next_block->size_status |= VM_PREVBUSY;
    }

    return (void *)((char *)fit + sizeof(struct block_header));

}
