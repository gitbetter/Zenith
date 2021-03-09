#version 450 core
#include "Shaders/Uniforms/camera.glsl" //! #include "../../../../_Assets/Shaders/Uniforms/camera.glsl"
#include "Shaders/Uniforms/object.glsl" //! #include "../../../../_Assets/Shaders/Uniforms/object.glsl"

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = ViewProjection * M * vec4(position, 1.0);
}
