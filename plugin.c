#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "plugin.h"
#include "memmory.h"

static Plugin* plugin = NULL;


Plugin* plugin_init(){
    TraceLog(LOG_INFO, "[PLUGIN] Plugin initialized!\n");
    plugin = malloc(sizeof(Plugin));
    assert(plugin != NULL && "Plugin malloc error!");
    memset(plugin, 0, sizeof(Plugin));
    // This variable look like memmory pages of os.
    // Initial page allocate all available memmory
    // then is any allocation request is coming
    // that page is spliting.
    arenas_insert(&plugin->freeArenas, 0, MAX_RAM_SIZE);

    return plugin;
}

Plugin* plugin_preload(){
    TraceLog(LOG_INFO, "[PLUGIN] Plugin preload run!\n");
    return plugin;
}

void plugin_postload(Plugin* p){
    TraceLog(LOG_INFO, "[PLUGIN] Plugin postload run!\n");
    plugin = p;
    stack_push(&plugin->stack, plugin->stack.sp);
    arenas_insert(&plugin->allocatedArenas, 0, 3);
    arenas_insert(&plugin->allocatedArenas, 3, 2);
    arenas_insert(&plugin->allocatedArenas, 5, 2);
    arenas_insert(&plugin->allocatedArenas, 8, 1);
    arenas_dump(&plugin->allocatedArenas, "allocated");
}

void DrawStack(){


}

void DrawHeap(){}

void plugin_task(){
    BeginDrawing();
        ClearBackground(BLACK);
        // Draw stack
        char size[30];
        sprintf(&size, "Stack(%li):", plugin->stack.sp);
        DrawText(size, 10, 10, 20, LIGHTGRAY);
        int xPadding = 40, heapCellHeight = 40, stackCellWidth=30, stackCellHeight =  40, cellPadding = 2;
        DrawRectangle(xPadding, stackCellHeight , GetScreenWidth() - 2* xPadding , heapCellHeight, ORANGE);
        if(plugin->stack.sp == 0){
            DrawText("Stack is empty", xPadding +20, 50, 20, LIGHTGRAY);
        } else{
            // Draw stack cells
            for(size_t i=0; i< plugin->stack.sp; i++){
                int cellX = (xPadding+ cellPadding *(i+1)) + (i* stackCellWidth),
                cellY = 40 +cellPadding;
                DrawRectangle(cellX, cellY, stackCellWidth, stackCellHeight - 2*cellPadding, RED);
                char stackData[8];
                sprintf(&stackData, "%li", (size_t)plugin->stack.data[i]);
                DrawText(stackData, cellX, cellY, stackCellHeight/2, LIGHTGRAY);
            }
        }

        // Draw heap
        int heapYPos = heapCellHeight + 40;
        int heapUsage =0;
        for(int i=0; i<plugin->allocatedArenas.count;i++){
            heapUsage += plugin->allocatedArenas.items[i].size;
        }
        sprintf(&size, "Heap(usage %i/%i):", heapUsage, MAX_RAM_SIZE);
        DrawText(size, 10, heapYPos + 40, 20, LIGHTGRAY);

        int heapGraphYPos= heapYPos+ heapCellHeight + 40,
        heapWidth = GetScreenWidth() - 2* xPadding,
        heapHeight = GetScreenHeight() - heapGraphYPos - xPadding,
        heapGridX = 32,
        heapGridY = MAX_RAM_SIZE / heapGridX,
        heapGridWidth = heapWidth / heapGridX,
        heapGridHeight = heapHeight / heapGridY;
        DrawRectangle(xPadding, heapGraphYPos,  heapWidth, heapHeight, ORANGE);
        // Draw memmory grid
        for(int x=0; x<heapGridX; x++){
            // vertical lines
            DrawLine(
                (x+1)*heapGridWidth + xPadding,
                heapGraphYPos,
                (x+1)*heapGridWidth + xPadding,
                heapGraphYPos +  heapHeight,
                BLACK
            );
        }
        // horizental lines
        for(int y=0; y<heapGridY; y++){
            DrawLine(
                xPadding,
                heapGraphYPos + (y+1)*heapGridHeight,
                heapWidth + xPadding,
                heapGraphYPos + (y+1)*heapGridHeight,
                BLACK
            );
        }

        // Draw allocated arenas
        for(size_t i=0; i< plugin->allocatedArenas.count; i++){
        const Arena arena = plugin->allocatedArenas.items[i];
        int y = arena.address /  heapGridX,
        x = arena.address %  heapGridX;
        DrawRectangle(
            xPadding + cellPadding + (x*heapGridWidth),
            heapGraphYPos + (y* heapCellHeight) + cellPadding,
            (heapWidth/heapGridX) * arena.size - (3*cellPadding),
            (heapHeight/heapGridY) - (3*cellPadding),
            RED
        ); 
        }

    EndDrawing();
}

