/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#include <stdio.h>
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

    // TODO: some null checking in the loop below
    struct aesd_buffer_entry *rtn_value = &buffer->entry[buffer->out_offs + 1 % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];

    int count = char_offset;
    int out_offs_count = buffer->out_offs;
    struct aesd_buffer_entry *counting_entry = rtn_value;
    int loops_completed = 0;
    while (count > -1) {
	count -= counting_entry->size;
	counting_entry = &buffer->entry[(out_offs_count + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
	out_offs_count++;
	loops_completed++;

    }
    counting_entry = &buffer->entry[(out_offs_count - 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
    printf("count: %d\n", count);
    printf("count + counting_entry->size: %d\n", count + (int)counting_entry->size);
    printf("loops completed: %d\n", loops_completed);

    if(loops_completed > AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED){

	*entry_offset_byte_rtn = 0;
    
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
    /**
    * TODO: implement per description
    */
    
    printf("buffer->in_offs: %d\n", buffer->in_offs);
    printf("buffer->out_offs: %d\n", buffer->out_offs);
    if( buffer->full && (buffer->in_offs) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED == buffer->out_offs)
    {
        printf("\n\n\nbuffer is full\n\n\n");
	buffer->out_offs = (buffer->in_offs + 1 ) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    }

    //struct aesd_buffer_entry *new_entry = malloc(sizeof(struct aesd_buffer_entry));
    //memcpy(new_entry, add_entry, sizeof(struct aesd_buffer_entry));
    buffer->entry[buffer->in_offs] = *add_entry;
    buffer->in_offs = (buffer->in_offs + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    
    if((buffer->in_offs) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED == buffer->out_offs) {
        buffer->full = 1;
    } else {
        buffer->full = 0;
    }

    return;
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    
    memset(buffer,0,sizeof(struct aesd_circular_buffer));

    //int write_start = buffer->in_offs;

    //int read_start = buffer->out_offs;


}
