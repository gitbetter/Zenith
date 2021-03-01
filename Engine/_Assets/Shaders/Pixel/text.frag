#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/ui.glsl" //! #include "../Uniforms/ui.glsl"

in VertexOutput vout;

out vec4 FragColor;

uniform sampler2D atlasSampler0;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(atlasSampler0, vout.FragUV).r);
  FragColor = color * sampled;
}
