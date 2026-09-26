#include "table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *entry_str(const Entry *e)  { return e->str; }
int entry_len(const Entry *e)          { return e->len; }
int entry_index(const Entry *e)        { return e->index; }

Table *table_create(void) {
    int expected_entries = 105;
    return table_create_sized(expected_entries);
}

Table *table_create_sized(int expected_entries) {
    Table *table = (Table*) malloc(sizeof(Table));
    
    if(table == NULL) {
        perror("Unable to initailize table.\n");
        exit(1);
    }

    int num_buckets = 2 * expected_entries + 1;
    table->buckets = (Entry**) calloc(num_buckets, sizeof(Entry*));
    if(table->buckets == NULL) {
        perror("Unable to allocate memory to table buckets.\n");
        exit(1);
    }

    table->num_buckets = num_buckets;
    table->num_entries = 0;

    return table;
}


void table_destroy(Table *t) {
    
    if(t == NULL) {
        perror("Refernece to \"NULL\" is passed.\n");
        exit(1);
    }

    Entry **buckets = t->buckets;
    int num_buckets = t->num_buckets;
    for(int i = 0; i < num_buckets; i++) {
        Entry *head = buckets[i];
        while(head != NULL) {
            Entry* toDelete = head;
            head = head->next;
            free(toDelete->str);
            free(toDelete);
        }
        buckets[i] = NULL;
    }

    free(buckets);
    free(t);
}

unsigned long hash(const char* s, int len) {
    unsigned long h = 5381;
    for (int i = 0; i < len; i++) {
        h = ((h << 5) + h) + (unsigned char) s[i];
    }
    return h;
}

Entry *table_add_entry(Table *t, const char *s, int len, size_t entry_size, bool *was_new) {
    if(t == NULL) {
        perror("Refernce to \"NULL\" passed to function table_add_entry.\n");
        exit(1);
    }
    unsigned idx = hash(s, len) % t->num_buckets;

    //look if the entry already exists or not ? 
    for(Entry *e = t->buckets[idx]; e != NULL; e = e->next) {
        if(e->len == len && memcmp(e->str, s, len) == 0) {
            if(was_new)
                *was_new = false;
            return e; // entry found 
        }
    }

    //entry dosen't exist
    Entry *e = malloc(entry_size);
    if(e == NULL) {
        perror("Unable to allocate memory to new entry.\n");
        exit(1);
    }

    e->str = (char*) malloc(sizeof(char) * (len + 1));
    if(e->str == NULL) {
        perror("malloc failed while allocation of Entry e->str!\n");
        exit(1);
    }

    memcpy(e->str, s, len); //(dest, src, len)
    e->str[len] = '\0';
    e->len = len;
    e->index = t->num_entries++;
    e->next = t->buckets[idx];
    t->buckets[idx] = e;
    if(was_new)
        *was_new = true;

    return e;
}