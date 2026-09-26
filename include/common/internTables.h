#ifndef INTERNTABLES_H
#define INTERNTABLES_H

#include "table.h"

typedef struct StringEntry {
    Entry base;
} StringEntry;

typedef struct IntEntry {
    Entry base;
} IntEntry;

typedef struct InternTables {
    Table *idTable;
    Table *strTable;
    Table *intTable;
} InternTables;

/**
 * @brief creates and initializes the required intern tables.
 * @returns pointer to the InterTables object.
 */
InternTables *internTables_create(void);

/**
 * @brief creates and initializes the intern tables with specific initial size.
 * @param expected_entries No. of entries that are expected to be there.
 * @returns pointer to the InterTables object.
 */
InternTables *internTables_create_sized(int expected_entries);

Entry *internTables_add_id(InternTables *it, const char *id, int len);
StringEntry *internTables_add_string(InternTables *it, const char *str_const, int len);
IntEntry *internTables_add_int(InternTables *it, const char *int_const, int len);

void internTables_destroy(InternTables *it);
#endif