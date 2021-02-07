#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 7) in mat4 instanceM;

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

// Grass animation uniforms
uniform float objectHeight = 1.0;
uniform float timestamp = 0.0;
uniform vec3 windDirection = vec3(1.0, 0.0, 0.0);
uniform float windStrength = 5.0;
uniform bool instanced = false;

vec4 CalculateTranslation(vec4 position, float time, vec3 windDirection, float windStrength);
float rand(vec2 co);

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
    vec4 posWorld = m * pos;
    vec3 T = normalize(mat3(m) * tangent);
    vec3 B = normalize(mat3(m) * bitangent);
    vec3 N = normalize(mat3(m) * norm);
    vs_out.FragNormal = N;
    vs_out.FragPos = vec3(posWorld);
    vs_out.FragTBN = mat3(T, B, N);
    vs_out.FragUV = texCoords;
    vs_out.FragPosLightSpace = P_lightSpace * vec4(vs_out.FragPos, 1.0);
    gl_Position = ViewProjection * posWorld;
}

vec4 CalculateTranslation(vec4 position, float time, vec3 windDirection, float windStrength)
{
    vec3 windTranslation = windDirection * windStrength;
    return vec4(position.xyz + windTranslation * sin(time * rand(vec2(instanceM[3][0], instanceM[3][2]))), 1.0);
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}