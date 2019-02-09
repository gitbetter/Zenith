#version 330 core

layout (location = 0) in vec4 vertex;

out VS_OUT {
  vec4 FragPos;
  vec2 FragUV;
} vs_out;

uniform mat4 M;
uniform mat4 P;

void main()
{
    vs_out.FragPos = M * vec4(vertex.xy, 0.0, 1.0);
    vs_out.FragUV = vertex.zw;
    gl_Position = normalize(P * vs_out.FragPos);
}
