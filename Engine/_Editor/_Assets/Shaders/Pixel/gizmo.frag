#version 450 core

#include "Shaders/common.glsl" //! #include "../../../../_Assets/Shaders/common.glsl"

out vec4 FragColor;

uniform Material materials[MAX_MATERIALS];
uniform int materialIndex;

void main() {
  FragColor = materials[materialIndex].albedo;
}
