#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/ui.glsl" //! #include "../Uniforms/ui.glsl"

layout (location = 0) in vec4 vertex;
layout (location = 1) in mat4 instanceM;

out VertexOutput vout;

void main()
{
    mat4 m = M_UI;
    if (instanced_UI)
    {
        m = instanceM;
    }
    vout.FragLocalPos = vec4(vertex.xy, 0.0, 1.0);
    vout.FragWorldPos = m * vout.FragLocalPos;
    vout.FragUV = vertex.zw;
    gl_Position = P_UI * vout.FragWorldPos;
}
