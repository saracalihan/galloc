#ifndef MEMMORY_H
#define MEMMORY_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_STACK_SIZE (256)
#define MAX_RAM_SIZE (256)
#define ARENAS_CAPACITY MAX_RAM_SIZE
#define WORD_SIZE 8
#define PACK

// STACK
typedef struct{
    void* data[MAX_STACK_SIZE];
    size_t sp;
    size_t fp;
} Stack;


size_t stack_push(Stack* stack, void* data){
    if(stack->sp == MAX_STACK_SIZE){
        perror("[STACK] Max stack size\n");
        exit(EXIT_FAILURE);
    }
    stack->data[(stack->sp)++] = data;

    return stack->sp;
}

void* stack_pop(Stack* stack){
    void* data = stack->data[stack->sp];
    stack->sp--;
    return data;
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

    arenas->count++;
    if(offset == arenas->count-1){
        arenas->items[offset] = arena;
        return;
    }
    // Shift items one step right then insert new areana
    memcpy(&arenas->items[offset+2], &arenas->items[offset+1], sizeof(Arena) * (arenas->count - offset - 1));
    // Arenas* temp = malloc(sizeof(Arenas));
    // memset(temp, 0, sizeof(Arenas));
    // memcpy(temp->items[0], temp->items[offset], offset)
    arenas->items[offset+1] = arena;

    return;
}

void arenas_dump(const Arenas *arena, const char *name)
{
    printf("%s Chunks (%zu):\n", name, arena->count);
    for (size_t i = 0; i < arena->count; ++i) {
        printf("  address: %i, size: %zu\n",
               (void*) arena->items[i].address,
               arena->items[i].size);
    }
}

void* g_alloc(Arenas* freeArenas, Arenas* allocatedArenas, size_t size){
    if(freeArenas->count ==0){
        TraceLog(LOG_ERROR, "[Galloc] freeArenas is full!");
        exit(1);
    }
    if(size <=0){
        return NULL;
    }

    #ifndef PACK
    if(size%WORD_SIZE !=0){
        size = ((size/WORD_SIZE) +1) * WORD_SIZE;
    }
    #endif

    assert(size < ARENAS_CAPACITY && "g_alloc max size error!");

    for(int i=0; i< freeArenas->count; i++){
        Arena arena = freeArenas->items[i];
        if(size<= arena.size){
            arenas_insert(allocatedArenas, arena.address, size);
            if(size<arena.size){
                arenas_insert(freeArenas, arena.address + size, arena.size - size);
            }

            // Delete freed arena
            if(i == 0){
                memcpy(
                    &freeArenas->items[0],
                    &freeArenas->items[1],
                    sizeof(Arena) * freeArenas->count -1
                );
            } else{
                memcpy(
                    &(freeArenas->items[i]),
                    &freeArenas->items[i+1],
                    sizeof(Arena) * (freeArenas->count - i-1)
                );
            }
                freeArenas->count--;

            // if(i ==0){
            //     freeArenas->count=0;
            //     return;
            // }

            // freeArenas->count--;
            return arena.address;
        }
    }
    return NULL;
}

void g_free(Arenas* freeArenas, Arenas* allocatedArenas, void* address){
    if(address<0)
        return;
     Arena *arena = &allocatedArenas->items[0];
    for(size_t i=0; i<allocatedArenas->count && address >= arena->address; i++ ){
        if(arena->address != address){
            arena++;
            continue;
        }

        arenas_insert(freeArenas, arena->address, arena->size);

        // Delete allocate arena
        if(i == 0){
            memcpy(
                &allocatedArenas->items[0],
                &allocatedArenas->items[1],
                sizeof(Arena) * allocatedArenas->count -1
            );
        } else{
            memcpy(
                &(allocatedArenas->items[i]),
                &allocatedArenas->items[i+1],
                sizeof(Arena) * (allocatedArenas->count - i-1)
            );
        }
            allocatedArenas->count--;
    }
}

#endif // MEMMORY_H