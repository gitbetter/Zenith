#version 460 core

layout (location = 0) in vec4 aPos;

out vec3 localPos;

uniform mat4 P;
uniform mat4 V;

void main()
{
  localPos = vec3(aPos);
  gl_Position = P * V * aPos;
}
