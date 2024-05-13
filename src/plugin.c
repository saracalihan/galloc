#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "plugin.h"
#include "memmory.h"

#define STACK_CELL_FONT_SIZE 10

static Plugin* plugin = NULL;
int frame=0;

// stack variables
 int xPadding = 40, heapCellHeight = 40, stackCellWidth= STACK_CELL_FONT_SIZE*3, stackCellHeight =  40, cellPadding = 2,stackContainerWidth = 848 - 2* 40;

// heap variables

// UI Elements

void stack_pointer_element(int step, const char* name, Color color){
    int cellX = xPadding + (( cellPadding + stackCellWidth) * step),
        cellY = xPadding - 5 ,
        stickTick = 2,
        stickHeight = 20;

    if(step<0){
        cellX = xPadding- stackCellWidth;
        char stepMsg[30] ={0};
        sprintf(stepMsg, "< %li", abs(step));
        DrawText(
            stepMsg,
            cellX +8,//MeasureText(stepMsg, 8) ,
            xPadding + (stackCellHeight/2) - 4,
            8,
            LIGHTGRAY
        );
    }

    // Stick
    DrawRectangle(
        cellX,
        cellY,
        stickTick,
        heapCellHeight + stickHeight,
        color
        );

    // Label container
    int spanPadding = 4,
        spanWidth = MeasureText(name, 16) + spanPadding*2,
        spanX = (cellX + ( stickTick/2 )) - ( spanWidth/2 ),
        spanY = heapCellHeight + stickHeight *2 + cellPadding*2;
    DrawRectangle(
        spanX,
        spanY,
        spanWidth,
        16 + cellPadding,
        color
    );

    // Label
    DrawText(
        name,
        spanX + spanPadding,
        spanY ,
        16,
        LIGHTGRAY
    );
}

void stack_cell_element(size_t step, const char* text){
int cellX = (xPadding+ cellPadding *(step+1)) + (step* stackCellWidth),
    cellY = 40 +cellPadding;

    DrawRectangle(
        cellX,
        cellY,
        stackCellWidth,
        stackCellHeight - 2*cellPadding,
        RED
    );

    DrawText(
        text,
        cellX+ stackCellWidth/2 -  ( MeasureText(text, STACK_CELL_FONT_SIZE)/2 ),
        cellY + stackCellHeight/2 - STACK_CELL_FONT_SIZE,
        STACK_CELL_FONT_SIZE,
    LIGHTGRAY);
}

void rule_list_element(){
    int xPos = 2*xPadding + stackContainerWidth,
        yPos = xPadding,
        width = 200 - xPadding,
        height = GetScreenHeight()- 2*xPadding;

    DrawRectangle(
        xPos,
        yPos,
        width,
        height,
        WHITE
    );

    for(size_t i=0;i< plugin->rules.count; i++){
        DrawText(
            plugin->rules.items[i],
            xPos + cellPadding,
            yPos + (16 *i) + cellPadding,
            18,
            i==0 ? RED : BLACK
        );
    }
}

void read_rules(){
    FILE *f = fopen("rules.txt", "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read, lineCount =0;

    if(NULL == f){
        // TraceLog(LOG_ERROR,"Rules can not imported!\n");
        printf("[ERROR] Rule import error: %m\n");
        exit(1);
    }
    while ((read = getline(&line, &len, f)) != -1) {
        lineCount++;
    }
    fseek(f, 0, SEEK_SET);
    plugin->rules.count = lineCount;
    plugin->rules.items = malloc(sizeof(char*)*lineCount);

    size_t i=0;
    while ((read = getline(&line, &len, f)) != -1) {
        plugin->rules.items[i] = malloc(sizeof(char)*read);
        strcpy(plugin->rules.items[i], line);
        i++;
    }
    TraceLog(LOG_INFO, "%i rules imported!", lineCount);
    fclose(f);
}

Plugin* plugin_init(){
    TraceLog(LOG_INFO, "[PLUGIN] Plugin initialized!");
    plugin = malloc(sizeof(Plugin));
    assert(plugin != NULL && "Plugin malloc error!");
    memset(plugin, 0, sizeof(Plugin));
    read_rules();
    // Initial page allocate all available memmory
    // then is any allocation request is coming
    // that page is spliting.
    arenas_insert(&plugin->freeArenas, 0, MAX_RAM_SIZE);
    return plugin;
}

// Logics

Plugin* plugin_preload(){
    TraceLog(LOG_INFO, "[PLUGIN] Plugin preload run!");
    return plugin;
}

void plugin_postload(Plugin* p){
    TraceLog(LOG_INFO, "[PLUGIN] Plugin postload run!");
    plugin = p;
    arenas_dump(&plugin->allocatedArenas, "allocated");
}




// GALLOC MEMORY FUNCTIONS
void galloc_stack_push(void* data){
    stack_push(&plugin->stack, data);
    TraceLog(LOG_INFO, "[GALLOC] Stack push %i!", data);
}

void* galloc_stack_pop(){
    TraceLog(LOG_INFO, "[GALLOC] Stack pop!");
    return stack_pop(&plugin->stack);
}

void galloc_create_scope(void* data){
    stack_push(&plugin->stack, plugin->stack.fp);
    plugin->stack.fp = plugin->stack.sp;
    stack_push(&plugin->stack, data);
    TraceLog(LOG_INFO, "[GALLOC] Scope created!");
}

void galloc_remove_scope(){
    while(plugin->stack.fp <= plugin->stack.sp && plugin->stack.sp !=0 ){
        stack_pop(&plugin->stack);
    }

    if(plugin->stack.fp !=0){
        plugin->stack.fp = stack_pop(&plugin->stack);
        plugin->stack.sp++;
    } else{
        plugin->stack.sp = 0;
    }
    TraceLog(LOG_INFO, "[GALLOC] Scope removed!");
}

void* galloc_malloc(size_t size){
    void* address = g_alloc(&plugin->freeArenas, &plugin->allocatedArenas, size);
    galloc_stack_push(address);
    TraceLog(LOG_INFO, "[GALLOC] Malloc size: %i, address: %i!", size, address);
    return address;
}

void galloc_free(void* address){
    g_free(&plugin->freeArenas, &plugin->allocatedArenas, address);
    TraceLog(LOG_INFO, "[GALLOC]  %i address freed!", address);
}

void process_command(const char* command){
    if(NULL != strstr(command, "dec")){
        char* p =strstr(command, "dec");
        p+=4;
        galloc_stack_push(atoi(p));
    } 
    else if(NULL != strstr(command, "malloc")){
        char* p =strstr(command, "malloc");
        p+=7;
        galloc_malloc(atoi(p));
    } else if(NULL != strstr(command, "free")){
        char* p =strstr(command, "free");
        p+=5;
        galloc_free(atoi(p));

    } else if(NULL != strstr(command, "scope")){
        char* p =strstr(command, "scope");
        p+=6;
        galloc_create_scope(atoi(p));
    } else if(NULL != strstr(command, "remove")){
        char* p =strstr(command, "remove");
        p+=7;
        galloc_remove_scope(atoi(p));
    }  else {
        TraceLog(LOG_ERROR,"invalid command!");
    }
}


void plugin_task(){
    static bool play=false;
    frame++;
    BeginDrawing();
        ClearBackground(BLACK); 
        if(IsKeyPressed(KEY_F)){
            for(int i=0;i< plugin->rules.count;i++){
                free(plugin->rules.items[i]);
            }
            memset(plugin, 0, sizeof(Plugin));
            arenas_insert(&plugin->freeArenas, 0, MAX_RAM_SIZE);
            TraceLog(LOG_INFO, "Flush the data");
            read_rules();
        }
        if(IsKeyPressed(KEY_C)){
            galloc_create_scope(8);
        }
         if(IsKeyPressed(KEY_ONE)){
            arenas_dump(&plugin->freeArenas, "free");
        }
        if(IsKeyPressed(KEY_TWO)){
            arenas_dump(&plugin->allocatedArenas, "allocated");
        }
        if(IsKeyPressed(KEY_V)){
            galloc_remove_scope();
        }
        if(IsKeyPressed(KEY_A)){
            stack_push(&plugin->stack, 1);
        }
        if(IsKeyPressed(KEY_M)){
            galloc_malloc(3);
        }
        if(IsKeyPressed(KEY_P)){
            play=!play;
            TraceLog(LOG_INFO, "[Plugin] play status is %i", play);
        }
        if(IsKeyPressed(KEY_N) || (play && frame % 30 == 0)){
            frame =0;
            if(plugin->rules.count !=0){
                process_command(*plugin->rules.items);
                free(*plugin->rules.items);
                plugin->rules.items++;
                plugin->rules.count--;
            }
        }

        // Draw stack
        char size[30];
        sprintf(&size, "Stack(%li):", plugin->stack.sp);
        DrawText(size, 10, 10, 20, LIGHTGRAY);
        DrawRectangle(
            xPadding,
            stackCellHeight,
            stackContainerWidth,
            heapCellHeight,
            ORANGE
        );

        if(plugin->stack.sp == 0){
            DrawText("Stack is empty", xPadding +20, 50, 20, LIGHTGRAY);
        } else{
            // Draw stack cells
            int cellCount = stackContainerWidth / (cellPadding+stackCellWidth);

            for(size_t i=0; i< cellCount && i< plugin->stack.sp ; i++){
                // Get last "cellCount" element
                int index = (
                    plugin->stack.sp<cellCount
                        ? 0
                        : (plugin->stack.sp - cellCount)
                    ) + i;

                char stackData[8];
                sprintf(&stackData, "%li", (size_t)plugin->stack.data[index]);
                stack_cell_element(i, stackData);
            }
            stack_pointer_element(plugin->stack.sp < cellCount ? plugin->stack.sp : cellCount ,"sp", BLUE);

            // Shift fp if it is not fit to the screen
            int fpPos = plugin->stack.sp < cellCount ? plugin->stack.fp :  (cellCount - plugin->stack.sp) + plugin->stack.fp;
            stack_pointer_element(fpPos,"fp", DARKGREEN);

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
        heapWidth = stackContainerWidth,
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

        // Draw user story

        rule_list_element();



    EndDrawing();
}
