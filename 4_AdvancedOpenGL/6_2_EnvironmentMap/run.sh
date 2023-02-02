#!/bin/zsh
g++ main.cpp -lGLWindowing -lShapes -lGLModelLoader -lGLShaderLoader -lGLTextureLoader -lGL -lGLEW
# qrenderdoc
./a.out
rm a.out