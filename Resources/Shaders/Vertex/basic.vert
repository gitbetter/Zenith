#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out VS_OUT {
  vec3 FragPos;
  vec3 FragNormal;
  vec2 FragUV;
  vec4 FragPosLightSpace;
} vs_out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 P_lightSpace;

void main()
{
    vs_out.FragPos = vec3(M * vec4(position, 1.0));
    vs_out.FragNormal = transpose(inverse(mat3(M))) * normal;
    vs_out.FragUV = texCoords;
    vs_out.FragPosLightSpace = P_lightSpace * vec4(vs_out.FragPos, 1.0);
    gl_Position = P * V * M * vec4(position, 1.0);
}
