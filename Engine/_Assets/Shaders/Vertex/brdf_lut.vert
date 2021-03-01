#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out VertexOutput vout;

void main()
{
  vout.FragUV = aTexCoords;
  gl_Position = vec4(aPos, 0.0, 1.0);
}
