#ifndef MM_MALLOC_H
#define MM_MALLOC_H

#include <stddef.h> // Para size_t

// Estructura de metadatos para los bloques de memoria
// Tienen libertad de modificarla, pero esta es una sugerencia base.
typedef struct block_meta {
    size_t size;
    struct block_meta *next;
    short free; // 1 si está libre, 0 si está ocupado
    int magic; // Para debugging (ej. 0x12345678)
} block_meta;

#define META_SIZE sizeof(struct block_meta)

// API Pública
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);
block_meta* split_block(block_meta* block, size_t size);

#endif
