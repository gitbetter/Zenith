#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out VS_OUT {
  vec3 FragPos;
  vec2 FragUV;
} vs_out;

uniform mat4 M;

void main()
{
    vs_out.FragPos = vec3(M * vec4(position, 1.0));
    vs_out.FragUV = texCoords;
    gl_Position = vec4(vs_out.FragPos, 1.0);
}
