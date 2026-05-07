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
    block_meta * const blocksBase = (block_meta*) base;
    assert(blocksBase == NULL || blocksBase->next == NULL || (blocksBase->next->prev == blocksBase));
    block_meta* newBlock = NULL;
    for(block_meta* it = blocksBase; it != NULL; it = it->next)
    {
        assert(it->magic == 0x12345678);
        assert(it->next == NULL || (it->next->prev == it));
        if(!it->free || it->size < size) continue;
        newBlock = split_block(it,size);
        if(newBlock == NULL) continue;
        assert(newBlock->next == NULL || (newBlock->next->prev == newBlock));
        newBlock->free = FALSE;
        return ((void*) newBlock) + BLOCK_META_SIZE;
    }
    const size_t pageSize = getpagesize();
    if(base == NULL)
    {
        base = sbrk(pageSize);
        if(base == (void *) -1) return NULL;
        newBlock = (block_meta*) base;
        newBlock->prev = newBlock;
    }
    else
    {
        newBlock = (block_meta*)sbrk(pageSize);
        if(newBlock == (block_meta*)((void *) -1)) return NULL;
        blocksBase->prev->next = newBlock;
        newBlock->prev = blocksBase->prev;
        blocksBase->prev = newBlock;
    }
    newBlock->size = pageSize - BLOCK_META_SIZE;
    newBlock->next = NULL;
    newBlock->magic = 0x12345678;
    assert(newBlock->next == NULL || (newBlock->next->prev == newBlock));
    newBlock = split_block(newBlock,size);
    assert(newBlock->next == NULL || (newBlock->next->prev == newBlock));
    if(newBlock == NULL) return NULL;
    newBlock->free = FALSE;
    void* ptr = ((void*) newBlock) + BLOCK_META_SIZE;
    return ptr;
}

void my_free(void *ptr) {
    // TODO: Marcar el bloque como libre.
    // TODO: Fusionar bloques adyacentes (Coalescing).
    block_meta* block = (block_meta*) (ptr - BLOCK_META_SIZE);
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
    if(block->next != NULL && block->next->free) coalesce(block);
    assert(block->next == NULL || (block->next->prev == block));
    if(block != ((block_meta*) base) && block->prev->free)coalesce(block->prev);
    assert(block->next == NULL || (block->next->prev == block));
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
        if(block->next == NULL || !block->next->free || block->next->size < (size_t) sizeDiff + BLOCK_META_SIZE)
        {
            void* newPtr = my_malloc(size);
            memcpy(newPtr,ptr,block->size);
            return newPtr;
        }
        if(split_block(block->next,sizeDiff) == NULL) return NULL;
        assert(block->next->next == NULL || (block->next->next->prev == block->next));
        coalesce(block);
        assert(block->next->next == NULL || (block->next->next->prev == block->next));
        return ptr;
    }
    else if(sizeDiff < 0)
    {
        block = split_block(block,size);
        assert(block->next == NULL || (block->next->prev == block));
        if(block == NULL) return NULL;
        coalesce(block->next);
        assert(block->next == NULL || (block->next->prev == block));
    }
    return ptr;
}

block_meta* split_block(block_meta* block, size_t size)
{
    assert(block->magic == 0x12345678);
    if(block->size == size) return block;
    if(block->size < (size + BLOCK_META_SIZE)) return NULL;
    block_meta* splitted = (block_meta*)(((void*) block) + BLOCK_META_SIZE + size);
    splitted->size = block->size - size - BLOCK_META_SIZE;
    splitted->next = block->next;
    if(splitted->next != NULL) splitted->next->prev = splitted;
    splitted->prev = block;
    splitted->free = TRUE;
    splitted->magic = 0x12345678;
    block->size = size;
    block->next = splitted;
    if(splitted->next == NULL) {((block_meta*) base)->prev = splitted;}
    assert(block->next->magic == 0x12345678);
    return block;
}

void coalesce(block_meta* block)
{
    assert(block->magic == 0x12345678);
    block_meta* nextBckp = block->next;
    if(nextBckp == NULL) return;
    block->size += BLOCK_META_SIZE + nextBckp->size;
    block->next = nextBckp->next;
    if(block->next == NULL) {((block_meta*) base)->prev = block;}
    else block->next->prev = block;
    nextBckp->next = NULL;
    nextBckp->prev = NULL;
}