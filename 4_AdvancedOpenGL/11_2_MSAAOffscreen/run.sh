#!/bin/zsh
g++ main.cpp -lGLWindowing -lGLShaderLoader -lGLTextureLoader -lFullscreenQuad -lGL -lGLEW
# qrenderdoc
./a.out
rm a.out