#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"
#include "Shaders/Uniforms/object.glsl" //! #include "../Uniforms/object.glsl"
#include "Shaders/Uniforms/model.glsl" //! #include "../Uniforms/model.glsl"
#include "Shaders/Uniforms/light.glsl" //! #include "../Uniforms/light.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in mat4 instanceM;

out VertexOutput vout;

void main()
{   
    vec4 pos = vec4(position, 1.0);
    mat4 m = (instanced ? instanceM : M);
    vec3 T = normalize(mat3(m) * tangent);
    vec3 B = normalize(mat3(m) * bitangent);
    vec3 N = normalize(mat3(m) * normal);
    vout.FragNormal = N;
    vout.FragTBN = mat3(T, B, N);
    if (rigged)
    {
        mat4 boneTransform = mat4(0.0);
        for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
        {
            if (boneIDs[i] == -1) continue;
            boneTransform += Bones[boneIDs[i]] * boneWeights[i];
        }
        vout.FragNormal = mat3(boneTransform) * vout.FragNormal;
        pos = boneTransform * pos;
    }
    vout.FragWorldPos = m * pos;
    vout.FragViewPos = V * vout.FragWorldPos;
    vout.FragUV = texCoords;
    for (int i = 0; i < NUM_SHADOW_CASCADES; i++)
    {
        vout.FragPosLightSpace[i] = ViewProjectionsLightSpace[i] * vout.FragWorldPos;
    }
    gl_Position = ViewProjection * vout.FragWorldPos;
}
