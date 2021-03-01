#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec3 position;

uniform mat4 P;
uniform mat4 V;

out VertexOutput vout;

void main()
{
  vout.FragLocalPos = vec4(position, 1.0);
  gl_Position = P * V * vout.FragLocalPos;
}
