RAYLIB_PATH=./raylib-5.0
RAYLIB_FLAGS=-I${RAYLIB_PATH}/src -L${RAYLIB_PATH}/src  -l:libraylib.so -Wl,-rpath=./raylib-5.0/src ${FLAGS}
FLAGS= -Wall -Wextra -lm -ldl -ggdb

all: build_main run

build_main: main.c build_plugin
	cc main.c ${RAYLIB_FLAGS} -o main

build_plugin: plugin.c
	cc -c -fPIC plugin.c ${RAYLIB_FLAGS} -o plugin.o
	cc plugin.o -shared ${RAYLIB_FLAGS} -o libplugin.so

run: main
	./main
