#version 450 core

in vec3 FragPos;
in vec3 FragNormal;
in vec3 FragUV;

out vec4 FragColor;

uniform vec4 color;

void main() {
  FragColor = color;
}
