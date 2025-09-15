#include "unity.h"
#include "aesd-circular-buffer.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_sane(void) {
    TEST_ASSERT_EQUAL(5, 5);
}

void test_init(void) {


    struct aesd_circular_buffer buffer;

    buffer.in_offs = buffer.out_offs = 0;

    aesd_circular_buffer_init(&buffer);

    for ( int i = 0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; i++ ) {
	TEST_ASSERT_EQUAL('\0', buffer.entry[i].buffptr);
    }

    TEST_ASSERT_EQUAL(0, buffer.in_offs);
    TEST_ASSERT_EQUAL(0, buffer.out_offs);
    TEST_ASSERT_EQUAL(0, buffer.full);

}


void test_add_entry(void) {
    struct aesd_buffer_entry A;
    struct aesd_buffer_entry B;
    struct aesd_buffer_entry C;

    const char c_A = 'A';
    const char c_B = 'B';
    const char c_C = 'C';

    A.buffptr = &c_A;
    B.buffptr = &c_B;
    C.buffptr = &c_C;

    struct aesd_circular_buffer buffer;

    buffer.in_offs = buffer.out_offs = 0;

    aesd_circular_buffer_init(&buffer);

    aesd_circular_buffer_add_entry(&buffer, &A);
    aesd_circular_buffer_add_entry(&buffer, &B);
    aesd_circular_buffer_add_entry(&buffer, &C);

    TEST_ASSERT_EQUAL('A', *buffer.entry[0].buffptr);
    TEST_ASSERT_EQUAL('B', *buffer.entry[1].buffptr);
    TEST_ASSERT_EQUAL('C', *buffer.entry[2].buffptr);

    for ( int i = 3; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; i++ ) {
        TEST_ASSERT_EQUAL('\0', buffer.entry[i].buffptr);
    }

}


void test_aesd_circular_buffer_find_entry_offset_for_fpos(void) {

    struct aesd_buffer_entry A;
    struct aesd_buffer_entry B;
    struct aesd_buffer_entry C;

    const char c_A = 'A';
    const char c_B = 'B';
    const char c_C = 'C';

    A.buffptr = &c_A;
    B.buffptr = &c_B;
    C.buffptr = &c_C;

    A.size = 1;
    B.size = 1;
    C.size = 1;

    struct aesd_circular_buffer buffer;

    buffer.in_offs = buffer.out_offs = 0;

    aesd_circular_buffer_init(&buffer);

    aesd_circular_buffer_add_entry(&buffer, &A);
/*
    aesd_circular_buffer_add_entry(&buffer, &B);
    aesd_circular_buffer_add_entry(&buffer, &C);

*/
    struct aesd_buffer_entry *rval;

    rval = malloc(sizeof(struct aesd_buffer_entry));

    aesd_circular_buffer_find_entry_offset_for_fpos(&buffer, 0, &rval);
/*
    TEST_ASSERT_EQUAL('A', rval->buffptr);
*/
}


void test_aesd_circular_buffer_find_entry_offset_string(void) {

    struct aesd_buffer_entry A;
    struct aesd_buffer_entry B;
    struct aesd_buffer_entry C;

    const char *c_A = "abcdefghijklm";

    A.buffptr = c_A;

    A.size = 13;

    struct aesd_circular_buffer buffer;

    buffer.in_offs = buffer.out_offs = 0;

    aesd_circular_buffer_init(&buffer);

    aesd_circular_buffer_add_entry(&buffer, &A);
    
    struct aesd_buffer_entry *rval;

    size_t char_offset;

    for ( int i = 0; i < 13; i++ ) {
        rval = aesd_circular_buffer_find_entry_offset_for_fpos(&buffer, i, &char_offset);

        //fwrite(char_offset, sizeof(char), 1, stdout);
	
	TEST_ASSERT_EQUAL(*(char*)char_offset, c_A[i]);
    }

}

