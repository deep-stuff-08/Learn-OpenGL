#!/bin/zsh
g++ main.cpp -lGLWindowing -lGLShaderLoader -lGLTextureLoader -lGL -lGLEW -lFullscreenQuad
./a.out
# qrenderdoc
rm a.out