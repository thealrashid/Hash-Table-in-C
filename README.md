# Hash Table in C

A lightweight, generic hash table implementation in C using `void*` keys and values, supporting dynamic resizing, custom hash and comparison functions, and collision resolution via chaining.

---

## 🧠 Features

- Customizable key and value types via function pointers
- Dynamic resizing (growth and shrinkage)
- Separate chaining for collision resolution
- Simple iterator API for traversal
- Optional cleanup functions for keys and values

---

## 📦 API Overview

```c
int hash_table_insert(HashTable *ht, void *key, void *value);
void *hash_table_get(HashTable *ht, const void *key);
int hash_table_remove(HashTable *ht, const void *key);
void hash_table_destroy(HashTable *ht);
```

## 🛠 Customization

You can define your own:
- `hash()` function (e.g., for strings, integers, structs)
- `compare()` function
- `free_key()` and `free_value()` for memory-managed data
