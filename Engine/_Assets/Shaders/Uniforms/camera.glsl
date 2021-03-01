//? #version 450 core

layout (std140, binding = 1) uniform Camera
{
    mat4 V;
    mat4 P;
    mat4 ViewProjection;
    mat4 PreviousViewProjection;
    mat4 InverseViewProjection;
    vec3 ViewPosition;
};