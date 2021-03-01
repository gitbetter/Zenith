#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

in VertexOutput vout;

out vec4 FragColor;

uniform vec4 color;

void main() {
  FragColor = color;
}
