#version 330 core

out vec4 FragColor;

in VS_OUT {
  vec4 FragPos;
  vec2 FragUV;
} fs_in;

uniform vec4 backgroundColor;

uniform sampler2D image0;

void main() {
  FragColor = backgroundColor;
}
