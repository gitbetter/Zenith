//? #version 450 core

layout (std140, binding = 6) uniform UI
{
    mat4 P_UI;
    mat4 M_UI;
    vec4 color;
    vec4 borderColor;
    vec2 pixelSize;
    float borderWidth;
    float borderRadius;
    bool instanced_UI;
};