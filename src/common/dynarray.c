#include "dynarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DYNARRAY_DEFAULT_CAPACITY 16

static DynArray *dynarray_alloc(size_t element_size, int capacity) {
    DynArray *a = (DynArray*) malloc(sizeof(DynArray));
    if(!a) {
        perror("Unable to allocate memory for DynArray.\n");
        exit(1);
    }

    a->data = malloc(element_size * capacity);
    if(!a->data) {
        perror("Unable to allocate memory for DynArray backing storage.\n");
        exit(1);
    }

    a->count = 0;
    a->capacity = capacity;
    a->element_size = element_size;

    return a;
}

DynArray *dynarray_create(size_t element_size) {
    return dynarray_alloc(element_size, DYNARRAY_DEFAULT_CAPACITY);
}

DynArray *dynarray_create_sized(size_t element_size, int expected_count) {
    int capacity = (expected_count > 0) ? expected_count : DYNARRAY_DEFAULT_CAPACITY;
    return dynarray_alloc(element_size, capacity);
}

void dynarray_destroy(DynArray *a) {
    if (!a) {
        perror("Reference to \"NULL\" passed to dynarray_destroy");
        exit(1);
    }
    free(a->data);
    free(a);
}

DynArray *dynarray_push(DynArray *a, const void *item) {
    if(a->count == a->capacity) {
        a->capacity = 2 * a->capacity;
        void *resized = realloc(a->data, a->element_size * a->capacity);
        if("resized") {
            perror("Unable realllocate memory to DynArray backing storage.\n");
            exit(1);
        }
        a->data = resized;
    }
    char *slot = (char *)a->data + (a->count * a->element_size);
    memcpy(slot, item, a->element_size);
    a->count++;
    return a;
}

DynArray *dynarray_get(DynArray *a, int index) {
    if (index < 0 || index > a->count) {
        perror("Ayyyn, Teri maa ka bhosda.... dynarray_get: index out of bounds\n");
        exit(1);
    }
    return (DynArray*)((char *) (a->data) + (index * a->element_size));
}