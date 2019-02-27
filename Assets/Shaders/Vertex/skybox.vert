#version 400 core

layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 P;
uniform mat4 V;

void main() {
  TexCoords = position;
  vec4 pos = P * V * vec4(position, 1.0);
  gl_Position = pos.xyww;
}
