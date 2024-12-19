library: build/renderer.a
build/renderer.a: shaders/texture-sapp.h build/renderer.o build/texture.o
	ar rvs build/renderer.a build/renderer.o build/texture.o

build/texture.o: src/texture.c
	gcc -c src/texture.c -o ./build/texture.o

build/renderer.o: src/renderer.c
	gcc -c src/renderer.c -o ./build/renderer.o

shader: shaders/texture-sapp.h
shaders/texture-sapp.h: shaders/texture-sapp.glsl
	./shaders/sokol-shdc --input ./shaders/texture-sapp.glsl --output ./shaders/texture-sapp.h --slang glsl430:hlsl5:metal_macos:wgsl

example: build/example
build/example: build/example.o build/renderer.a
	gcc build/example.o build/renderer.a -o build/example -lX11 -lXcursor -lXi -lGL -lm

build/example.o: examples/example.c
	gcc -c examples/example.c -o ./build/example.o

clean:
	rm -f ./build/*
	rm ./shaders/texture-sapp.h
