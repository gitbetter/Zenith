#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = ViewProjection * vec4(position, 1.0);
}
