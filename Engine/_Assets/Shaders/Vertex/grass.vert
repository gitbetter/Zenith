#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 7) in mat4 instanceM;

out VertexOutput vout;

uniform mat4 M;
uniform mat4 V;
uniform mat4 ViewProjection;
uniform mat4 ViewProjectionLightSpace[NUM_SHADOW_CASCADES];

// Grass animation uniforms
uniform float objectHeight = 1.0;
uniform float timestamp = 0.0;
uniform vec3 windDirection = vec3(1.0, 0.0, 0.0);
uniform float windStrength = 5.0;
uniform bool instanced = false;

vec4 CalculateTranslation(vec4 position, float time, vec3 windDirection, float windStrength);

void main()
{   
    vec4 pos = vec4(position, 1.0);
    vec3 norm = normal;
    // Animate upper vertices of grass objects
    if (texCoords.y <= 0.1) {
        vec4 translation = CalculateTranslation(pos, timestamp, windDirection, windStrength);
        pos = pos + translation;
        norm = normalize(norm * objectHeight + translation.xyz);
    }
    mat4 m = (instanced ? instanceM : M);
    vout.FragWorldPos = m * pos;
    vec3 T = normalize(mat3(m) * tangent);
    vec3 B = normalize(mat3(m) * bitangent);
    vec3 N = normalize(mat3(m) * norm);
    vout.FragViewPos = V * vout.FragWorldPos;
    vout.FragNormal = N;
    vout.FragTBN = mat3(T, B, N);
    vout.FragUV = texCoords;
    for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
        vout.FragPosLightSpace[i] = ViewProjectionLightSpace[i] * vout.FragWorldPos;
    }
    gl_Position = ViewProjection * vout.FragWorldPos;
}

vec4 CalculateTranslation(vec4 position, float time, vec3 windDirection, float windStrength)
{
    vec3 windTranslation = windDirection * windStrength;
    return vec4(position.xyz + windTranslation * sin(time * Random(vec2(instanceM[3][0], instanceM[3][2]))), 1.0);
}