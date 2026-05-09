#ifndef MM_MALLOC_H
#define MM_MALLOC_H

#include <stddef.h> // Para size_t

// Estructura de metadatos para los bloques de memoria
// Tienen libertad de modificarla, pero esta es una sugerencia base.
typedef struct block_meta {
    size_t size;
    struct block_meta *next;
    struct block_meta *prev; //Apuntador al nodo anterior en la lista. En el primer nodo, apunta al último elemento de la lista para acceder a este rápidamente
    short free; // 1 si está libre, 0 si está ocupado
    int magic; // Para debugging (ej. 0x12345678)
} block_meta;

#define META_SIZE sizeof(struct block_meta)

// API Pública
void *my_malloc(size_t size) ;
void my_free(void *ptr);
void *my_calloc(size_t nmemb, size_t size);
void *my_realloc(void *ptr, size_t size);

/*Dados un apuntador a un bloque y un tamaño, la función reduce el tamaño del bloque a size, y crea uno nuevo con el tamaño restante
del bloque original menos el tamaño de los metadatos del nuevo bloque en la siguiente posición en la lista. Si termina con éxito, retorna
el mismo apuntador al bloque pasado, ahora con tamaño size, de lo contrario, retorna NULL*/
block_meta* split_block(block_meta* block, size_t size);

/*Dado un bloque, aumenta su tamaño sumándole el tamaño del siguiente y de sus metadatos, luego elimina el bloque siguiente de la lista. Sólo debe ser llamada si el siguiente bloque en la lista está libre*/
void coalesce(block_meta* block);

#endif
