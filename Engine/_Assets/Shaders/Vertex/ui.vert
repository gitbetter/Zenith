#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec4 vertex;
layout (location = 1) in mat4 instanceM;

out VertexOutput vout;

uniform mat4 M;
uniform mat4 P;
uniform bool instanced = false;

void main()
{
    mat4 m = M;
    if (instanced) {
        m = instanceM;
    }
    vout.FragLocalPos = vec4(vertex.xy, 0.0, 1.0);
    vout.FragPos = m * vout.FragLocalPos;
    vout.FragUV = vertex.zw;
    gl_Position = P * vout.FragPos;
}
