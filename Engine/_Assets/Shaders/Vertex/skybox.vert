#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"

layout (location = 0) in vec3 position;

out VertexOutput vout;

void main() {
  vout.FragLocalPos = vec4(position, 1.0);
  mat4 rotView = mat4(mat3(V));
  vec4 pos = P * rotView * vout.FragLocalPos;
  gl_Position = pos.xyww;
}
