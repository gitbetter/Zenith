#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec3 aPos;

out VertexOutput vout;

uniform mat4 P;
uniform mat4 V;

void main() {
  vout.FragLocalPos = vec4(aPos, 1.0);
  mat4 rotView = mat4(mat3(V));
  vec4 pos = P * rotView * vout.FragLocalPos;
  gl_Position = pos.xyww;
}
