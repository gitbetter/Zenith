#version 400 core
layout (location = 0) in vec3 position;

uniform mat4 P;
uniform mat4 V;

void main()
{
    gl_Position = P * V * mat4(1.0) * vec4(position, 1.0);
}
