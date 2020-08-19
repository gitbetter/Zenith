#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 P;
uniform mat4 V;

void main() {
  localPos = aPos;
  mat4 rotView = mat4(mat3(V));
  vec4 pos = P * rotView * vec4(aPos, 1.0);
  gl_Position = pos.xyww;
}
