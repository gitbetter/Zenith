#version 450 core

layout (location = 0) in vec4 vertex;
layout (location = 1) in mat4 instanceM;

out VS_OUT {
  vec4 FragPos;
  vec2 FragUV;
} vs_out;

uniform mat4 M;
uniform mat4 P;
uniform bool instanced = false;

void main()
{
    mat4 m = M;
    if (instanced) {
        m = instanceM;
    }
    vs_out.FragPos = m * vec4(vertex.xy, 0.0, 1.0);
    vs_out.FragUV = vertex.zw;
    gl_Position = P * vs_out.FragPos;
}
