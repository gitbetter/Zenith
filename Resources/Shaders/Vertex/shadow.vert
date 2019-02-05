#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragUV;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    vec4 viewPos = V * M * vec4(position, 1.0);
    FragPos = vec3(viewPos);
    FragNormal = normalize(mat3(transpose(inverse(M))) * normal);
    FragUV = texCoords;

    gl_Position = P * viewPos;
}
