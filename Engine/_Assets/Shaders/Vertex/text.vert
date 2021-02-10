#version 450 core

layout (location = 0) in vec4 vertex;

out vec2 UV;

uniform mat4 P;

void main()
{
    UV = vertex.zw;
    gl_Position = P * vec4(vertex.xy, 0.0, 1.0);
}
