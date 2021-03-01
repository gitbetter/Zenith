#version 450 core

layout (location = 0) in vec3 position;

uniform mat4 M;
uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * M * vec4(position, 1.0);
}
