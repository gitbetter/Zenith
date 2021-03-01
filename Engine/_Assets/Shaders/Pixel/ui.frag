#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform vec4 color;
uniform vec4 borderColor;
uniform vec2 resolution;
uniform float borderWidth = 0.0;
uniform float borderRadius = 0.0;

uniform sampler2D colorSampler0;

void main() {
    // Calculate border radius distance and discard fragment if not within bounds
    vec2 pos = (abs(vout.FragUV - 0.5) + 0.5) * resolution;
    vec2 arc_cpt = max(pos - resolution + borderRadius, 0.0);
    if (length(arc_cpt) > borderRadius)
        discard;

    float maxX = resolution.x - borderWidth;
    float minX = borderWidth;
    float maxY = resolution.y - borderWidth;
    float minY = borderWidth;

    if (pos.x < maxX && pos.x > minX &&
        pos.y < maxY && pos.y > minY) {
        vec4 texColor = texture(colorSampler0, vout.FragUV);
        vec4 outColor = color * texColor;
        if (outColor.a < 0.1)
            discard;
        FragColor = outColor;
    } else {
        FragColor = borderColor;
    }
}