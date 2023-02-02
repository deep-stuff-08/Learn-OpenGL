#!/bin/zsh
g++ main.cpp -lGLWindowing -lGLShaderLoader -lGLTextureLoader -lGL -lGLEW
# qrenderdoc
./a.out
rm a.out