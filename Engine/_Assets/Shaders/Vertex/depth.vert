#version 450 core

layout (location = 0) in vec3 position;
layout (location = 7) in mat4 instanceM;

uniform mat4 ViewProjection;
uniform mat4 M;
uniform bool instanced = false;

void main()
{
    gl_Position = ViewProjection * (instanced ? instanceM : M) * vec4(position, 1.0);
}
