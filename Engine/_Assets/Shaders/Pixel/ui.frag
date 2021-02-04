#version 450 core

out vec4 FragColor;

in VS_OUT {
  vec4 FragPos;
  vec2 FragUV;
} fs_in;

uniform vec4 color;
uniform vec4 borderColor;
uniform vec2 resolution;
uniform float aspectRatio = 1.0;
uniform float borderWidth = 0.0;
uniform float borderRadius = 0.0;

uniform sampler2D colorSampler0;

void main() {
    // Calculate border radius distance and discard fragment if not within bounds
    vec2 pos = (abs(fs_in.FragUV - 0.5) + 0.5) * resolution;
    vec2 arc_cpt = max(pos - resolution + borderRadius, 0.0);
    if (length(arc_cpt) > borderRadius)
        discard;

    float maxX = 1.0 - borderWidth;
    float minX = borderWidth;
    float maxY = 1.0 - borderWidth / aspectRatio;
    float minY = borderWidth / aspectRatio;

    if (fs_in.FragUV.x < maxX && fs_in.FragUV.x > minX &&
        fs_in.FragUV.y < maxY && fs_in.FragUV.y > minY) {
        vec4 texColor = texture(colorSampler0, fs_in.FragUV);
        vec4 outColor = color * texColor;
        if (outColor.a < 0.1)
            discard;
        FragColor = outColor;
    } else {
        FragColor = borderColor;
    }
}