#version 460 core

in vec2 UV;
out vec4 FragColor;

uniform sampler2D glyph;
uniform vec4 color;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyph, UV).r);
  FragColor = color * sampled;
}
