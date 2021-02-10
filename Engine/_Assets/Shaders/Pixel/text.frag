#version 450 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D atlas0;
uniform vec4 color;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(atlas0, UV).r);
  FragColor = color * sampled;
}
