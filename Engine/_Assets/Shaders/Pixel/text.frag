#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

in VertexOutput vout;

out vec4 FragColor;

uniform sampler2D atlas0;
uniform vec4 color;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(atlas0, vout.FragUV).r);
  FragColor = color * sampled;
}
