#version 460 core

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 uv;

out vec2 UV;

uniform mat4 P;

void main()
{
    UV = uv;
    gl_Position = P * vec4(vertex, 0.0, 1.0);
}
