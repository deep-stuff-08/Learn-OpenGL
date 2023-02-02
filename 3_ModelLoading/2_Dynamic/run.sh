#!/bin/zsh
g++ main.cpp -lassimp -lLighting -lGLWindowing -lGLModelLoader -lGLShaderLoader -lGLTextureLoader -lGLEW -lGL -g
# qrenderdoc
# gdb a.out
./a.out
rm a.out
