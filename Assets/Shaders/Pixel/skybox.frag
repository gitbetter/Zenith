#version 400 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube cubemap1;

void main() {
  FragColor = texture(cubemap1, TexCoords);
}
