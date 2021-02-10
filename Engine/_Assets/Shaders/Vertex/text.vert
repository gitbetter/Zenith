#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec4 vertex;

out VertexOutput vout;

uniform mat4 P;

void main()
{
    vout.FragUV = vertex.zw;
    gl_Position = P * vec4(vertex.xy, 0.0, 1.0);
}
