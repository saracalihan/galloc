#ifndef PLUGIN_H
#define PLUGIN_H
#include <stddef.h>
#include <raylib.h>
#include <string.h>

#include "memmory.h"

typedef struct{
    Stack stack;
    Arenas allocatedArenas;
    Arenas freeArenas;
} Plugin;


// void plugin_init();
// Plugin* plugin_preload();
// void plugin_load(Plugin*);
// Plugin* plugin_postload();
// void plugin_task();

#endif // PLUGIN_H
