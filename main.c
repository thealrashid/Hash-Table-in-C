#include <stdio.h>
#include "hashtable.h"

size_t int_hash(const void *key) {
    int k = *(const int *)key;

    // Simple hashing using bit-mixing (32-bit)
    size_t hash = (size_t)k;
    hash ^= (hash >> 20) ^ (hash >> 12);
    hash ^= (hash >> 7) ^ (hash >> 4);

    return hash;
}


int int_compare(const void *a, const void *b) {
    int int_a = *(const int *)a;
    int int_b = *(const int *)b;
    return (int_a > int_b) - (int_a < int_b);
}


int main() {
    HashTable *ht = hash_table_create(20, int_hash, int_compare, NULL, NULL);

    int *messi = (int *)malloc(sizeof(int));
    *messi = 10;
    hash_table_insert(ht, messi, "Messi");

    int *ronaldo = (int *)malloc(sizeof(int));
    *ronaldo = 7;
    hash_table_insert(ht, ronaldo, "Ronaldo");
    
    int *neymar = (int *)malloc(sizeof(int));
    *neymar = 11;
    hash_table_insert(ht, neymar, "Neymar");

    int *suarez = (int *)malloc(sizeof(int));
    *suarez = 10;
    hash_table_insert(ht, suarez, "Suarez");

    char *seven = (char *)hash_table_get(ht, ronaldo);

    printf("Seven: %s\n", seven);

    hash_table_destroy(ht);
    
    return 0;
}