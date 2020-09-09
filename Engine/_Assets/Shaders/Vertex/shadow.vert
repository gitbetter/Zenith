#version 460 core

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in mat4 instanceM;

const int MAX_BONES = 100;

uniform mat4 P_lightSpace;
uniform mat4 M;
uniform mat4 Bones[MAX_BONES];
uniform bool rigged = false;
uniform bool instanced = false;

void main()
{
	vec4 pos = vec4(position, 1.0);
    if (rigged) {
	    mat4 boneTransform = Bones[boneIDs[0]] * boneWeights[0];
		boneTransform += Bones[boneIDs[1]] * boneWeights[1];
		boneTransform += Bones[boneIDs[2]] * boneWeights[2];
		boneTransform += Bones[boneIDs[3]] * boneWeights[3];

        pos = boneTransform * pos;
    }

    gl_Position = P_lightSpace * (instanced ? instanceM : M) * pos;
}
