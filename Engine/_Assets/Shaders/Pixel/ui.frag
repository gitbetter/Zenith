#version 460 core

out vec4 FragColor;

in VS_OUT {
  vec4 FragPos;
  vec2 FragUV;
} fs_in;

uniform vec4 color;
uniform vec4 borderColor;
uniform float aspectRatio = 1.0;
uniform float borderWidth = 0.0;

uniform sampler2D image;

void main() {
    float maxX = 1.0 - borderWidth;
    float minX = borderWidth;
    float maxY = maxX / aspectRatio;
    float minY = minX / aspectRatio;

    if (fs_in.FragUV.x < maxX && fs_in.FragUV.x > minX &&
        fs_in.FragUV.y < maxY && fs_in.FragUV.y > minY) {
        FragColor = color * texture(image, fs_in.FragUV);
    } else {
        FragColor = borderColor;
    }
}
