INCLUDES= -I ./include
OBJECTS= ./build/chip8.o ./build/memory.o ./build/stack.o ./build/keyboard.o ./build/screen.o

FLAGS= -g

all: ${OBJECTS}
	gcc ${FLAGS} ${INCLUDES} ./src/main.c ${OBJECTS} -L ./lib -lmingw32 -lSDL2main -lSDL2 -o ./bin/main

./build/memory.o:src/memory.c
	gcc ${FLAGS} ${INCLUDES} ./src/memory.c -c -o ./build/memory.o

./build/stack.o:src/stack.c
	gcc ${FLAGS} ${INCLUDES} ./src/stack.c -c -o ./build/stack.o

./build/keyboard.o:src/keyboard.c
	gcc ${FLAGS} ${INCLUDES} ./src/keyboard.c -c -o ./build/keyboard.o

./build/screen.o:src/screen.c
	gcc ${FLAGS} ${INCLUDES} ./src/screen.c -c -o ./build/screen.o

./build/chip8.o:src/chip8.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8.c -c -o ./build/chip8.o



run:
	./bin/main



runinvaders:
	./bin/main ./roms/INVADERS

runpong:
	./bin/main ./roms/PONG

runmissile:
	./bin/main ./roms/MISSILE

runtetris:
	./bin/main ./roms/TETRIS

runufo:
	./bin/main ./roms/UFO

runtank:
	./bin/main ./roms/TANK




clean:
	rm ./bin/main
	rm ./build/*
