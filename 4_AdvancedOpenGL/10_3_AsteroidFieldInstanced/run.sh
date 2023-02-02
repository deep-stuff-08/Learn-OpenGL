#!/bin/zsh
g++ main.cpp -lGLWindowing -lGLShaderLoader -lGLModelLoader -lGLEW -lGL
# qrenderdoc
./a.out
rm a.out
