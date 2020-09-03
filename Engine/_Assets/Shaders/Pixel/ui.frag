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
    float maxY = 1.0 - borderWidth / aspectRatio;
    float minY = borderWidth / aspectRatio;

    if (fs_in.FragUV.x < maxX && fs_in.FragUV.x > minX &&
        fs_in.FragUV.y < maxY && fs_in.FragUV.y > minY) {
        vec4 texColor = texture(image, fs_in.FragUV);
        if (texColor.a < 0.1)
            discard;
        FragColor = color * texColor;
    } else {
        FragColor = borderColor;
    }
}
