#include "internTables.h"
#include <stdio.h>
#include <stdlib.h>

InternTables *internTables_create(void) {
    int expected_entries = 103;
    return internTables_create_sized(expected_entries);
}

InternTables *internTables_create_sized(int expected_entries) {
    InternTables *it = (InternTables*) malloc(sizeof(InternTables));
    if(it == NULL) {
        perror("Unable to initialize interntable.\n");
        exit(1);
    }

    it->idTable = table_create_sized(expected_entries);
    it->intTable = table_create_sized(expected_entries);
    it->strTable = table_create_sized(expected_entries);

    return it;
}

void internTables_destroy(InternTables *it) {
    if(it == NULL) {
        perror("Reference to \"NULL\" is passed!\n");
        exit(1);
    }

    table_destroy(it->idTable);
    table_destroy(it->intTable);
    table_destroy(it->strTable);

    free(it);
}

Entry *internTables_add_id(InternTables *it, const char *id, int len) {
    bool was_new;
    return table_add_entry(it->idTable, id, len, sizeof(Entry), &was_new);
}

StringEntry *internTables_add_string(InternTables *it, const char *str_const, int len) {
    bool was_new;
    return (StringEntry*) table_add_entry(it->strTable, str_const, len, sizeof(StringEntry), &was_new);
}

IntEntry *internTables_add_int(InternTables *it, const char *int_const, int len) {
    bool was_new;
    return (IntEntry*) table_add_entry(it->intTable, int_const, len, sizeof(IntEntry), &was_new);
}