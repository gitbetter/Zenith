#version 400 core
layout (location = 0) in vec3 position;

uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * mat4(1.0) * vec4(position, 1.0);
}
