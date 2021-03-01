#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/ui.glsl" //! #include "../Uniforms/ui.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform sampler2D colorSampler0;

void main() {
    // Calculate border radius distance and discard fragment if not within bounds
    vec2 pos = (abs(vout.FragUV - 0.5) + 0.5) * pixelSize;
    vec2 arc_cpt = max(pos - pixelSize + borderRadius, 0.0);
    if (length(arc_cpt) > borderRadius)
        discard;

    float maxX = pixelSize.x - borderWidth;
    float minX = borderWidth;
    float maxY = pixelSize.y - borderWidth;
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