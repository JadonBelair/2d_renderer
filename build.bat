.\shaders\sokol-shdc.exe --input ./shaders/texture-sapp.glsl --output ./shaders/texture-sapp.h --slang glsl430:hlsl5:metal_macos:wgsl
cl /Fe: "build/" /Fo: "build/" ./examples/example.c ./src/*.c
