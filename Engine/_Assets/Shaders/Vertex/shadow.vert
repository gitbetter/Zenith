#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/object.glsl" //! #include "../Uniforms/object.glsl"
#include "Shaders/Uniforms/model.glsl" //! #include "../Uniforms/model.glsl"
#include "Shaders/Uniforms/light.glsl" //! #include "../Uniforms/light.glsl"

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in mat4 instanceM;

void main()
{
	vec4 pos = vec4(position, 1.0);
    if (rigged) {
        mat4 boneTransform = mat4(0.0);
        for (int i = 0; i < MAX_BONES_PER_VERTEX; i++) {
            if (boneIDs[i] == -1) continue;
            boneTransform += Bones[boneIDs[i]] * boneWeights[i];
        }
        pos = boneTransform * pos;
    }

    gl_Position = ViewProjectionLightSpace * (instanced ? instanceM : M) * pos;
}
