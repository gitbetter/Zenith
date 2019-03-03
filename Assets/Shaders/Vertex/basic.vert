#version 400 core

layout (location = 0) in vec4 aPos;

out vec3 localPos;

uniform mat4 V;
uniform mat4 P;

void main()
{
  localPos = vec3(aPos);
  gl_Position = P * V * aPos;
}
