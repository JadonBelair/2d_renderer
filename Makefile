all: build/main.o build/texture.o build/renderer.o
	gcc -o ./build/main ./build/*.o -lX11 -lXcursor -lXi -lGL -lm

build/main.o: src/main.c
	gcc -c src/main.c -o ./build/main.o

build/texture.o: src/texture.c
	gcc -c src/texture.c -o ./build/texture.o

build/renderer.o: src/renderer.c
	gcc -c src/renderer.c -o ./build/renderer.o

clean:
	rm -f ./build/*
