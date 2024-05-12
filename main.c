#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <dlfcn.h>

#include "plugin.h"

#define LOAD_SYM(name) \
name = dlsym(plugin_handler, #name); \
    if (!name) { \
        fprintf(stderr, "%s\n", dlerror()); \
        exit(EXIT_FAILURE); \
    } \
    TraceLog(LOG_INFO, "--------------------------------");\
    TraceLog(LOG_INFO, #name" function initialized");\
    TraceLog(LOG_INFO, "--------------------------------");\

#define PLUGIN_PATH "./libplugin.so"

void* plugin_handler = NULL;
void* (*plugin_init)(void) = NULL;
void* (*plugin_preload)(void) = NULL;
void (*plugin_postload)(void*) = NULL;
void (*plugin_task)() = NULL;
void* plugin_data = NULL;

void load_plugin(){
    plugin_init = NULL;plugin_postload=NULL;plugin_task=NULL;plugin_preload=NULL;

    if(plugin_handler !=NULL){
        dlclose(plugin_handler);
    }
    plugin_handler = dlopen(PLUGIN_PATH, RTLD_NOW);
     if (!plugin_handler) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    TraceLog(LOG_INFO, "--------------------------------");
    TraceLog(LOG_INFO, "Plugin initialized");
    TraceLog(LOG_INFO, "--------------------------------");

    LOAD_SYM(plugin_init)
    LOAD_SYM(plugin_preload)
    LOAD_SYM(plugin_postload)
    LOAD_SYM(plugin_task)
}
 
int main(){
    // Enable hotreload with using dynamic library
    load_plugin();
    plugin_data = plugin_init();

    InitWindow(848 + 200, 448, "galloc");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_R)){
            TraceLog(LOG_INFO, "Reload");
            void* data = plugin_preload();
            plugin_data = data;
            load_plugin();
            plugin_postload(data);
        }
        plugin_task();
    }

    CloseWindow();
    return 0;
}