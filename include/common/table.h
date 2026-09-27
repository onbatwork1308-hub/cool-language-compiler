#ifndef TABLE_H
#define TABLE_H

#include "utils.h"
#include <stddef.h>

/**
 * @struct Entry
 * @brief Base entry: what Every Table's entry shares
 */
typedef struct Entry {
    char* str;              /** Entered text, heap-owned copy */
    int len;                /** length in bytes (don't relly on '\0') */
    int index;              /** creation order/ codgen lables */

    struct Entry *next;     /** collision chain within a bucket */
} Entry;

/**
 * Entry utilities
 */
const char *entry_str(const Entry *e);
int  entry_len(const Entry *e);
int entry_index(const Entry *e);


/**
 * @struct Table
 * @brief Base Table: a generic hash table of internal Entries
 * @details Uses Chaining based implementation of HashMap, intentionally as
 * the number of entries in a given program don't have any bound. Hence the
 * teqhniques like probing which have bound on the array size, will not be 
 * efficient.
 */

typedef struct Table { 
    Entry **buckets;

    int num_buckets;
    int num_entries;    /** also doubles as the "next Index" counter */
} Table;

/**
 * @brief creates a table and initializes the table.
 * @returns pointer to the table created. 
 */
Table *table_create(void);

/**
 * @brief create a table which fits the expected size to avoid later 
 * resizing overhead.
 * @param expexted_entries No. of entries which are expected to be e
 * -registered in the table.
 * @returns pointer to the table created.
 */
Table *table_create_sized(int expected_entries);

/**
 * @brief destroys the table pointed by the Table pointer *t
 * @param t pointer to the table which is to be destroyed.
 */
void table_destroy(Table *t);



/**
 * @brief adds new given string to the table.
 * @details If a matching string already exists, returns its existing 
 * Entry*.If a matching string already exists, returns its existing E
 * -ntry*.  
 * @param t Table* pointer to the table in which entry is to be added.
 * @param s const char* pointer to the string which is to be entered i
 * -n the table.
 * @param len int length of the string.
 * @param entry_size size of the entry in bytes.
 * @param was_new pointer to variable used by caller.
 *  
 * @pre table exists
 * @post new entry is added to the table, if entry already exists was_new
 * set to false otherwise true
 * @returns pointer to the added entry in table.
 */
Entry *table_add_entry(Table *t, const char *s, int len, size_t entry_size, bool *was_new);

/**
 * @brief will be defined and used in future, igonre for now.
 */
Entry *table_insert(Table *t, void *key, void *value);

/**
 * @brief hash function for creating hashing indices for the entries.
 * 
 * @details this hashing function internally uses djb2 hashing also 
 * called as (Dan Bernstein's hash) to generate the hashing index.
 * This ensures derministic behavior and avoids avalanche effect.
 * multiplying by 33 (h*32 + h) mixes bits across the whole accumul
 * -ator quickly over a handful of characters, so short strings (wh
 * -ich is exactly what you're hashing) get well-scattered outputs. 
 * It's also just two operations per character — extremely cheap.
 * 
 * @param s const char* pointer to the string whose entry is to be c
 * -reated.
 * @param len length of the string s.
 * 
 * @returns unsigned long hashing index for s.
 */
unsigned long hash(const char* s, int len);
#endif