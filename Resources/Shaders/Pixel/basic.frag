#version 330 core

in vec3 FragPos;
in vec3 FragNormal;
in vec3 FragUV;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 baseColor;

void main() {
  FragColor = vec4(lightColor * baseColor, 1.0);
}
