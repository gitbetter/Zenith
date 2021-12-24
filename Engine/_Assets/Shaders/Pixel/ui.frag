#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/ui.glsl" //! #include "../Uniforms/ui.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform sampler2D colorSampler0;

void main()
{
    vec2 fragPos = (abs(vout.FragUV - 0.5) + 0.5) * pixelSize;
    vec2 bottomLeft = step(vec2(borderWidth), fragPos);
    vec2 topRight = step(vec2(borderWidth), pixelSize - fragPos);
    float pct = bottomLeft.x * bottomLeft.y * topRight.x * topRight.y;

    vec4 texColor = texture(colorSampler0, vout.FragUV);
    vec4 outColor = vec4(vec3(1.0 - pct), 1.0) * borderColor + vec4(vec3(pct), 1.0) * color * texColor;

    if (outColor.a < 0.1)
    {
        discard;
    }

    FragColor = outColor;
}