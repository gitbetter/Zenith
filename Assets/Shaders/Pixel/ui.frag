#version 400 core

out vec4 FragColor;

in VS_OUT {
  vec4 FragPos;
  vec2 FragUV;
} fs_in;

uniform vec4 color;

uniform sampler2D image;

void main() {
  FragColor = color * texture(image, fs_in.FragUV);
}
