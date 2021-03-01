#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec4 aPos;

out VertexOutput vout;

uniform mat4 P;
uniform mat4 V;

void main()
{
  vout.FragLocalPos = aPos;
  gl_Position = P * V * aPos;
}
