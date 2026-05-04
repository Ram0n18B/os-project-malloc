#include <unistd.h> // Para sbrk
#include <stdio.h>
#include <assert.h>
#include "include/mm_malloc.h"

#define true 1
#define false 0

// Inicio de la lista enlazada del heap
void *base = NULL;

void *my_malloc(size_t size) {
    // TODO: Implementar First-Fit o Best-Fit
    // 1. Verificar si hay un bloque libre del tamaño adecuado.
    // 2. Si no, pedir espacio al OS con sbrk().
    size_t blockMetaSize = sizeof(block_meta);
    block_meta* aux = NULL;
    size_t pageSize = getpagesize();
    if(base == NULL)
    {
        base = sbrk(pageSize);
        aux = (block_meta*) base;
        aux->size = pageSize - blockMetaSize;
        aux->next = NULL;
        aux->free = true;
        aux->magic = 12345678;
    }
    block_meta* newBlock = NULL;
    for(block_meta* it = (block_meta*) base; it != NULL; it = it->next)
    {
        assert(it->magic == 12345678);
        aux = it;
        if(!it->free || it->size < size) continue;
        newBlock = split_block(it,size);
        newBlock->free = false;
        return ((void*) newBlock) + blockMetaSize;
    }
    aux->next = sbrk(pageSize);
    if(aux->next == (void *) -1) return NULL;
    newBlock = split_block(aux->next,size);
    newBlock->free = false;
}

void my_free(void *ptr) {
    // TODO: Marcar el bloque como libre.
    // TODO: Fusionar bloques adyacentes (Coalescing).
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    return NULL;
}

void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.
    return NULL;
}

block_meta* split_block(block_meta* block, size_t size)
{
    assert(block->magic == 12345678);
    if(block->size < size)
    {
        return NULL;
    }
    size_t blockMetaSize = sizeof(block_meta);
    block_meta* splitted = (block_meta*)(((void*) block) + blockMetaSize + size);
    splitted->size = block->size - size - blockMetaSize;
    splitted->next = block->next;
    splitted->free = true;
    splitted->magic = 12345678;
    block->size = size;
    block->next = splitted;
    return block;
}