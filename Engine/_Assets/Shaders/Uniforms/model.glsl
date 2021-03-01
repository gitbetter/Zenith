//? #version 450 core
//? #include "../constants.glsl"

layout (std140, binding = 3) uniform Model
{
    mat4 Bones[MAX_BONES_PER_MODEL];
    bool rigged;
    bool instanced;
};