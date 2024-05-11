#ifndef MEMMORY_H
#define MEMMORY_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_STACK_SIZE (256)
#define MAX_RAM_SIZE (256)
#define ARENAS_CAPACITY MAX_RAM_SIZE

// STACK
typedef struct{
    void* data[MAX_STACK_SIZE];
    size_t sp;
} Stack;


size_t stack_push(Stack* stack, void* data){
    if(stack->sp == MAX_STACK_SIZE){
        perror("[STACK] Max stack size\n");
        exit(EXIT_FAILURE);
    }
    stack->data[++(stack->sp)] = data;

    return stack->sp;
}

void* stack_pop(Stack* stack){
    return stack->data[(stack->sp)--];
}



// HEAP
static char* phisicalMemmory[MAX_RAM_SIZE];

typedef struct {
    size_t size;
    size_t address;
}Arena;

typedef struct{
    size_t count;
    Arena items[ARENAS_CAPACITY];
} Arenas;



void arenas_insert(Arenas* arenas, size_t address, size_t size){
    assert(arenas->count<ARENAS_CAPACITY && "arenas_insert over capacity");

    Arena arena = {
        .size=size,
        .address=address,
    };
    if(arenas->count == 0){
        arenas->count++;
        arenas->items[0] = arena;
        return;
    }
    // short list with address
    int offset =0;
    for(size_t i=0; i<arenas->count; i++){
        if(arenas->items[i].address < address){
            offset++;
            continue;
        }
    }
    // Shift items one step right then insert new areana
    memcpy(&arenas->items[offset+2], &arenas->items[offset+2], arenas->count - offset - 1);
    arenas->items[offset+1] = arena;
    arenas->count++;
    return;
}

void arenas_dump(const Arenas *arena, const char *name)
{
    printf("%s Chunks (%zu):\n", name, arena->count);
    for (size_t i = 0; i < arena->count; ++i) {
        printf("  start: %p, size: %zu\n",
               (void*) arena->items[i].address,
               arena->items[i].size);
    }
}

// void* g_alloc(size_t size){
//     if(size <=0){
//         return NULL;
//     }

//     assert(size < ARENAS_CAPACITY && "g_alloc max size error!");

//     for(int i=0; i< freeArenas.count; i++){
//         Arena arena = freeArenas.items[i];

//         if(size<= arena.size){
            
//         }
//     }
// }

#endif // MEMMORY_H