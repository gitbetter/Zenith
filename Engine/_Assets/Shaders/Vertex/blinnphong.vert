#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in mat4 instanceM;

out VertexOutput vout;

uniform mat4 M;
uniform mat4 ViewProjection;
uniform mat4 P_lightSpace;
uniform mat4 Bones[MAX_BONES];
uniform bool rigged = false;
uniform bool instanced = false;

void main()
{   
    vec4 pos = vec4(position, 1.0);
    mat4 m = (instanced ? instanceM : M);
    vec3 T = normalize(mat3(m) * tangent);
    vec3 B = normalize(mat3(m) * bitangent);
    vec3 N = normalize(mat3(m) * normal);
    vout.FragNormal = N;
    vout.FragTBN = mat3(T, B, N);
    if (rigged) {
        mat4 boneTransform = Bones[boneIDs[0]] * boneWeights[0];
        boneTransform += Bones[boneIDs[1]] * boneWeights[1];
        boneTransform += Bones[boneIDs[2]] * boneWeights[2];
        boneTransform += Bones[boneIDs[3]] * boneWeights[3];
        vout.FragNormal = mat3(boneTransform) * vout.FragNormal;
        pos = boneTransform * pos;
    }
    vout.FragPos = m * pos;
    vout.FragUV = texCoords;
    vout.FragPosLightSpace = P_lightSpace * vout.FragPos;
    gl_Position = ViewProjection * vout.FragPos;
}
