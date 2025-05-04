/**
 * This is a simple C header file which implements hash table in C for any data type.
 * All function definitions are available here.
 * Simply copy this header file to your directory and include "hashtable.h" in your code.
 * Create a hash table with hash_table_create API and provide the hash, compare, free_key, and free_value
 * APIs according to their arguments and return type. 
 */


#ifndef HASHTABLE_H
#define HASHTABLE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef size_t (*hash_func)(const void *); // Compute the hash value for key
typedef int (*compare_func)(const void *, const void *); // Compare the new key with existing keys, return 0 if true
typedef void (*free_key_func)(void *); // Free the key
typedef void (*free_value_func)(void *); // Free the value


/**
 * Hash Entry structure
 * Hold the key-value pair, pointer to the next entry and stores the computed hash value to avoid recomputation.
 */
typedef struct HashEntry {
    void *key;
    void *value;
    size_t hash_value;
    struct HashEntry *next;
} HashEntry;

/**
 * Hash Table structure.
 * Holds buckets, an array of HashEntry pointers that hold the entries. Index the buckets array is calculated by doing hash value
 * modulus of capacity. Each pointer in the array holds the head of the linked list of entries of that bucket.
 */
typedef struct HashTable {
    HashEntry **buckets;
    size_t size;
    size_t capacity;
    hash_func hash;
    compare_func compare;
    free_key_func free_key;
    free_value_func free_value;
} HashTable;


extern HashTable *hash_table_create(size_t capacity, hash_func hash, compare_func compare, free_key_func free_key, free_value_func free_value);
static int hash_table_resize(HashTable *ht, size_t new_capacity);
extern int hash_table_insert(HashTable *ht, void *key, void *value);
extern int hash_table_remove(HashTable *ht, const void *key);
extern void *hash_table_get(const HashTable *ht, const void *key);
extern void hash_table_destroy(HashTable *ht);


HashTable *hash_table_create(size_t capacity, hash_func hash, compare_func compare, free_key_func free_key, free_value_func free_value) {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));

    if (!ht) {
        fprintf(stderr, "Failed to allcate memory for hash table\n");
        return NULL;
    }

    capacity = capacity < 16 ? 16 : capacity;
    ht->buckets = (HashEntry **)calloc(capacity, sizeof(HashEntry *));
    if (!ht->buckets) {
        fprintf(stderr, "Failed to allcate memory for hash table buckets\n");
        return NULL;
    }

    ht->size = 0;
    ht->capacity = capacity;
    ht->hash = hash;
    ht->compare = compare;
    ht->free_key = free_key;
    ht->free_value = free_value;

    return ht;
}

static int hash_table_resize(HashTable *ht, size_t new_capacity) {
    HashEntry **new_buckets = (HashEntry **)calloc(new_capacity, sizeof(HashEntry *));
    if (!new_buckets) {
        fprintf(stderr, "Failed to allocate new buckets\n");
        return -1;
    }

    for (size_t i = 0; i < ht->capacity; i++) {
        HashEntry *entry = ht->buckets[i];
        while (entry) {
            HashEntry *next = entry->next;
            size_t new_index = entry->hash_value % new_capacity;
            entry->next = new_buckets[new_index];
            new_buckets[new_index] = entry;
            entry = next;
        }
    }

    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->capacity = new_capacity;

    return 0;
}

int hash_table_insert(HashTable *ht, void *key, void *value) {
    if (!ht) {
        fprintf(stderr, "Hash table doesn't exist\n");
        return -1;
    }
    if (!key) {
        fprintf(stderr, "Key is NULL");
        return -1;
    }

    if (ht->size >= ht->capacity * 0.7) {
        size_t new_capacity = ht->capacity * 2;
        if (hash_table_resize(ht, new_capacity) != 0) {
            fprintf(stderr, "Failed to resize the memory for hash table\n");
            return -1;
        }
    }

    size_t hash_value = ht->hash(key);
    size_t index = hash_value % ht->capacity;

    HashEntry *entry = ht->buckets[index];
    HashEntry *prev = NULL;

    while (entry) {
        if (entry->hash_value == hash_value && ht->compare(entry->key, key) == 0) {
            if (ht->free_value) ht->free_value(entry->value);
            entry->value = value;
            return 0;
        }
        prev = entry;
        entry = entry->next;
    }

    HashEntry *new_entry = (HashEntry *)malloc(sizeof(HashEntry));
    if(!new_entry) {
        fprintf(stderr, "Failed to new entry for hash table");
        return -1;
    }

    new_entry->key = key;
    new_entry->value = value;
    new_entry->hash_value = hash_value;
    new_entry->next = NULL;

    if (prev) prev->next = new_entry;
    else ht->buckets[index] = new_entry;

    ht->size++;

    return 0;
}

int hash_table_remove(HashTable *ht, const void *key) {
    if (!ht) {
        fprintf(stderr, "Hash table doesn't exist\n");
        return -1;
    }
    if (!key) {
        fprintf(stderr, "Key is NULL");
        return -1;
    }

    size_t hash_value = ht->hash(key);
    size_t index = hash_value % ht->capacity;

    HashEntry *entry = ht->buckets[index];
    HashEntry *prev = NULL;

    while (entry) {
        if (entry->hash_value == hash_value && ht->compare(entry->key, key) == 0) {
            if (prev) prev->next = entry->next;
            else ht->buckets[index] = entry->next;

            if (ht->free_key) ht->free_key(entry->key);
            if (ht->free_value) ht->free_value(entry->value);

            free(entry);
            ht->size--;

            if (ht->capacity > 16 && ht->size <= ht->capacity * 0.25) hash_table_resize(ht, ht->capacity / 2);

            return 0;
        }

        prev = entry;
        entry = entry->next;
    }

    return -1;
}

void *hash_table_get(const HashTable *ht, const void *key) {
    if (!ht) {
        fprintf(stderr, "Hash table doesn't exist\n");
        return NULL;
    }
    if (!key) {
        fprintf(stderr, "Key is NULL");
        return NULL;
    }

    size_t hash_value = ht->hash(key);
    size_t index = hash_value % ht->capacity;

    HashEntry *entry = ht->buckets[index];
    while (entry) {
        if (entry->hash_value == hash_value && ht->compare(entry->key, key) == 0)
            return entry->value;
        entry = entry->next;
    }
    return NULL;
}

void hash_table_destroy(HashTable *ht) {
    if (!ht) {
        fprintf(stderr, "Hash table doesn't exist\n");
        return;
    }

    for (size_t i = 0; i < ht->capacity; i++) {
        HashEntry *entry = ht->buckets[i];
        while (entry) {
            HashEntry *next = entry->next;
            if (ht->free_key) ht->free_key(entry->key);
            if (ht->free_value) ht->free_value(entry->value);
            free(entry);
            entry = next;
        }
    }

    free(ht->buckets);
    free(ht);
}


#endif /* HASHTABLE_H */