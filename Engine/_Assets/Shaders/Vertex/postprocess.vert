#version 450 core

layout (location = 0) in vec2 vertex;

out vec2 FragUV;

void main()
{
    FragUV = (vertex + vec2(1.0, 1.0)) / 2.0;
    gl_Position = vec4(vertex, 0.0, 1.0);
}
