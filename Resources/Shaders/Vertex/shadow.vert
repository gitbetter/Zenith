#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 P_lightSpace;
uniform mat4 M;

void main()
{
    gl_Position = P_lightSpace * M * vec4(position, 1.0);
}
