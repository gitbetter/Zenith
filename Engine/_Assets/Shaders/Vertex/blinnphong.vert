#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;

const int MAX_BONES = 100;

out VS_OUT {
    vec3 FragPos;
    vec3 FragNormal;
    vec2 FragUV;
    mat3 FragTBN;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 M;
uniform mat4 ViewProjection;
uniform mat4 P_lightSpace;
uniform mat4 Bones[MAX_BONES];
uniform bool rigged = false;

void main()
{
    mat4 boneTransform = Bones[boneIDs[0]] * boneWeights[0];
    boneTransform += Bones[boneIDs[1]] * boneWeights[1];
    boneTransform += Bones[boneIDs[2]] * boneWeights[2];
    boneTransform += Bones[boneIDs[3]] * boneWeights[3];
    
    vec3 T = normalize(mat3(M) * tangent);
    vec3 B = normalize(mat3(M) * bitangent);
    vec3 N = normalize(mat3(M) * normal);
    
    vec4 pos = vec4(position, 1.0);
    if (rigged) {
        pos = boneTransform * pos;
    }
    vs_out.FragPos = vec3(M * pos);
    vs_out.FragNormal = N;
    if (rigged) {
        vs_out.FragNormal = mat3(boneTransform) * vs_out.FragNormal;
    }
    vs_out.FragUV = texCoords;
    vs_out.FragTBN = mat3(T, B, N);
    vs_out.FragPosLightSpace = P_lightSpace * vec4(vs_out.FragPos, 1.0);
    gl_Position = ViewProjection * M * pos;
}
