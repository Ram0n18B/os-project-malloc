#include <unistd.h> // Para sbrk
#include <stdio.h>
#include <assert.h>
#include <string.h>
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
    size_t blockMetaSize = sizeof(block_meta);
    for(block_meta* it = (block_meta*) base; it != NULL; it = it->next)
    {
        if(((void*) it) + blockMetaSize != ptr) continue;
        if(it->free)
        {
            perror("my_free: Pointer was already freed\n");
            _exit(134);
        }
        it->free = true;
        coalesce(it);
        return;
    }
    perror("my_free(): Invalid pointer\n");
    _exit(134);
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    size_t total_size = nmemb*size;
    void* ptr = my_malloc(total_size);
    memset(ptr,0,total_size);
    block_meta* block = (block_meta*)(ptr - sizeof(block_meta));
    assert(block->magic == 12345678);
    return ptr;
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

void coalesce(block_meta* block)
{
    assert(block->next->magic == 12345678 && block->magic == 12345678);
    if(!block->next->free) return;
    block->size += sizeof(block_meta) + block->next->size;
    block->next = block->next->next;
}