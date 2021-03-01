#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec2 vertex;

out VertexOutput vout;

void main()
{
    vout.FragUV = (vertex + vec2(1.0, 1.0)) / 2.0;
    gl_Position = vec4(vertex, 0.0, 1.0);
}
