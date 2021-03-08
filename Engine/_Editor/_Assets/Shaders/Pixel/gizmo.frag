#include "Shaders/common.glsl" //! #include "../../../../_Assets/Shaders/common.glsl"
#include "Shaders/Uniforms/material.glsl" //! #include "../../../../_Assets/Shaders/Uniforms/material.glsl"

out vec4 FragColor;

void main() {
  FragColor = material.albedo;
}
