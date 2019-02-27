#version 400 core

in vec3 FragPos;
in vec3 FragNormal;
in vec3 FragUV;

out vec4 FragColor;

uniform vec4 highlightColor;

void main() {
  FragColor = highlightColor;
}
