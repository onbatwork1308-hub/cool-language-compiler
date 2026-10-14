#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdio.h>

/**
 * @struct DynArray
 * @brief Generic growable array. Stores element by value (memcpy'd in),
 * doubling capacity on overflow. Element type is opaque to this structure
 * the caller supplies element_size at creation and is responsible for 
 * the interpreting the bytes it gets back from dynarray_get.   
 */
typedef struct  DynArray {
    void *data;
    int count;
    int capacity;
    size_t element_size;
} DynArray;

/**
 * @brief create a dynarray for elements of the given size.
 * @param element_size size in bytes of one element which is 
 * to be stored in the dynarray.
 * @returns pointer to the created dynarray.
 */

DynArray *dynarray_create(size_t element_size);

/**
 * @brief creates a dynarray sized up front for an expected element count,
 * to avoid early resizing overhead.
 */
DynArray *dynarray_create_sized(size_t element_size, int expected_count);

/**
 * @brief destroys the dynarray.
 * @details does NOT free anything the elements themselves point to
 * if elements are the pointers to the heap data, the caller remains 
 * responsible for freeing what they point to, before or after calling
 * this function.
 */
void dynarray_destroy(DynArray *a);

/**
 * @brief appends one element to the end of the array, growing if needed.
 * @param item pointer to the element to copy in (element_size bytes are 
 * read from this pointer).
 * @returns the pointer to the old/reallocated dynarray.
 */
DynArray *dynarray_push(DynArray *a, const void *item);

/**
 * @brief returns a pointer to the element at the given index.
 * @details the returned pointer is only valid until the next
 * dynarray_push that triggers a resize -callers needing stable
 * reference should copy the value out, not hold the pointer.
 */

DynArray *dynarray_get(DynArray *a, int index);
#endif