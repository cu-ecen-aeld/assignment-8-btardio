/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Brandon Tardio
 * @date 2025-09-28
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.

 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end

 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.

 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(
		struct aesd_circular_buffer *buffer,
    		size_t char_offset, 
		size_t *entry_offset_byte_rtn 
		)
{

    struct aesd_buffer_entry *rtn_value = &buffer->entry[buffer->out_offs + 1 % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
    int count = char_offset;
    int out_offs_count = buffer->out_offs;
    struct aesd_buffer_entry *counting_entry = rtn_value;
    int loops_completed = 0;
    

    if( char_offset < 0 ) {
	*entry_offset_byte_rtn = -1;
	return NULL;
    }

    while (count > -1) {
	
	count -= counting_entry->size;
	counting_entry = &buffer->entry[(out_offs_count + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
	out_offs_count++;
	loops_completed++;

    }
    
    counting_entry = &buffer->entry[(out_offs_count - 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];

    if(loops_completed > AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED){

	*entry_offset_byte_rtn = -1;
    
        return NULL;

    } else {
    
	*entry_offset_byte_rtn = count + counting_entry->size;
    
    	return &buffer->entry[(out_offs_count - 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
    }
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{

    printk(KERN_WARNING "adding entry: %s\n", add_entry->buffptr);
    printk(KERN_WARNING "adding entry size: %d\n", add_entry->size);

    if (add_entry == NULL){
        return;
    }

    if( buffer->full && (buffer->in_offs) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED == buffer->out_offs)
    {
	buffer->out_offs = (buffer->in_offs + 1 ) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    }
 
#ifdef __KERNEL__   
    kfree(buffer->entry[buffer->in_offs].buffptr);
#else

    // test is using stack, but keeping above b/c user can't manage slab
    if (buffer->entry[buffer->in_offs].buffptr != NULL) { 
    	//free(buffer->entry[buffer->in_offs].buffptr);
    }

#endif
    
    buffer->entry[buffer->in_offs].buffptr = add_entry->buffptr;
    buffer->entry[buffer->in_offs].size = add_entry->size;

    printk(KERN_WARNING "buffer->entry[buffer->in_offs].buffptr: %s\n", buffer->entry[buffer->in_offs].buffptr);

    buffer->in_offs = (buffer->in_offs + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    
    if((buffer->in_offs) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED == buffer->out_offs) {
        buffer->full = 1;
    } else {
        buffer->full = 0;
    }

    buffer->count++;

    return;
}



/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    
    memset(buffer,0,sizeof(struct aesd_circular_buffer));

}
