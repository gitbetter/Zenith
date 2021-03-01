#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/object.glsl" //! #include "../Uniforms/object.glsl"
#include "Shaders/Uniforms/model.glsl" //! #include "../Uniforms/model.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"
#include "Shaders/Uniforms/light.glsl" //! #include "../Uniforms/light.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 7) in mat4 instanceM;

out VertexOutput vout;

// Grass animation uniforms
layout (std140, binding = USER_UBO0) uniform Grass
{
    float objectHeight;
    float timestamp;
    float windStrength;
    vec4 windDirection;
};

vec4 CalculateTranslation(vec4 position, float time, vec4 windDirection, float windStrength);

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
        vout.FragPosLightSpace[i] = ViewProjectionsLightSpace[i] * vout.FragWorldPos;
    }
    gl_Position = ViewProjection * vout.FragWorldPos;
}

vec4 CalculateTranslation(vec4 position, float time, vec4 windDirection, float windStrength)
{
    vec3 windTranslation = windDirection.xyz * windStrength;
    return vec4(position.xyz + windTranslation * sin(time * Random(vec2(instanceM[3][0], instanceM[3][2]))), 1.0);
}