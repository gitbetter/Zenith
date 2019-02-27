#version 400 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D glyph0;
uniform vec4 color;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyph0, UV).r);
  FragColor = color * sampled;
}
