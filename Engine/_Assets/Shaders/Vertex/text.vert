#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/ui.glsl" //! #include "../Uniforms/ui.glsl"

layout (location = 0) in vec4 vertex;

out VertexOutput vout;

void main()
{
    vout.FragUV = vertex.zw;
    gl_Position = P_UI * vec4(vertex.xy, 0.0, 1.0);
}
