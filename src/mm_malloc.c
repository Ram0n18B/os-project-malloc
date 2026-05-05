#include <unistd.h> // Para sbrk
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "mm_malloc.h"

#define TRUE 1
#define FALSE 0

#define BLOCK_META_SIZE sizeof(block_meta)

// Inicio de la lista enlazada del heap
void *base = NULL;

void *my_malloc(size_t size) {
    // TODO: Implementar First-Fit o Best-Fit
    // 1. Verificar si hay un bloque libre del tamaño adecuado.
    // 2. Si no, pedir espacio al OS con sbrk().
    block_meta* aux = NULL;
    block_meta* newBlock = NULL;
    for(block_meta* it = (block_meta*) base; it != NULL; it = it->next)
    {
        assert(it->magic == 0x12345678);
        aux = it;
        if(!it->free || it->size < size) continue;
        newBlock = split_block(it,size);
        newBlock->free = FALSE;
        return ((void*) newBlock) + BLOCK_META_SIZE;
    }
    const size_t pageSize = getpagesize();
    if(base == NULL)
    {
        base = sbrk(pageSize);
        newBlock = (block_meta*) base;
    }
    else
    {
        aux->next = (block_meta*)sbrk(pageSize);
        newBlock = aux->next;
    }
    if(newBlock == (block_meta*)((void *) -1)) return NULL;
    newBlock->size = pageSize - BLOCK_META_SIZE;
    newBlock->next = NULL;
    newBlock->free = TRUE;
    newBlock->magic = 0x12345678;
    newBlock = split_block(newBlock,size);
    newBlock->free = FALSE;
    return ((void*) newBlock) + BLOCK_META_SIZE;
}

void my_free(void *ptr) {
    // TODO: Marcar el bloque como libre.
    // TODO: Fusionar bloques adyacentes (Coalescing).
    block_meta* block = (block_meta*) ptr - BLOCK_META_SIZE;
    if(block->magic != 0x12345678)
    { 
        perror("my_free(): Invalid pointer\n");
        _exit(134);
    }
    if(block->free)
    {
        perror("my_free(): Pointer was already freed\n");
        _exit(134);
    }
    block->free = TRUE;
    coalesce(block);
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    const size_t total_size = nmemb*size;
    void* ptr = my_malloc(total_size);
    memset(ptr,0,total_size);
    block_meta* block = (block_meta*)(ptr - BLOCK_META_SIZE);
    assert(block->magic == 0x12345678);
    return ptr;
}

void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.
    block_meta* block = (block_meta*)(ptr - BLOCK_META_SIZE);
    assert(block->magic == 0x12345678);
    int sizeDiff = size - block->size;
    if(sizeDiff > 0)
    {
        if(block->next == NULL || !block->next->free || block->next->size < (size_t) sizeDiff)
        {
            void* newPtr = my_malloc(size);
            memcpy(newPtr,ptr,block->size);
            return newPtr;
        }
        block_meta* aux = (block_meta*)((void*)block->next + sizeDiff);
        memmove((void *) aux,(void*) block->next,BLOCK_META_SIZE);
        aux->size = block->next->size - sizeDiff;
        block->next = aux;
        return ptr;
    }
    else if(sizeDiff < 0)
    {
        block = split_block(block,size);
        if(block == NULL) return block;
        coalesce(block->next);
    }
    return ptr;
}

block_meta* split_block(block_meta* block, size_t size)
{
    assert(block->magic == 0x12345678);
    if(block->size < size)
    {
        return NULL;
    }
    block_meta* splitted = (block_meta*)(((void*) block) + BLOCK_META_SIZE + size);
    splitted->size = block->size - size - BLOCK_META_SIZE;
    splitted->next = block->next;
    splitted->free = TRUE;
    splitted->magic = 0x12345678;
    block->size = size;
    block->next = splitted;
    assert(block->next->magic == 0x12345678);
    return block;
}

void coalesce(block_meta* block)
{
    assert(block->magic == 0x12345678 && block->next->magic == 0x12345678);
    if(block->next == NULL|| !block->next->free) return;
    block->size += BLOCK_META_SIZE + block->next->size;
    block->next = block->next->next;
}