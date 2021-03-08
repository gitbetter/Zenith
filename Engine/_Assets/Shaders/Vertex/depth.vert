#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"
#include "Shaders/Uniforms/object.glsl" //! #include "../Uniforms/object.glsl"
#include "Shaders/Uniforms/model.glsl" //! #include "../Uniforms/model.glsl"

layout (location = 0) in vec3 position;
layout (location = 7) in mat4 instanceM;

void main()
{
    gl_Position = ViewProjection * (instanced ? instanceM : M) * vec4(position, 1.0);
}
