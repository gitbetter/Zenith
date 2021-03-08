#include "Shaders/common.glsl" //! #include "../common.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform samplerCube cubemapSampler0;

void main() {
  vec3 envColor = textureLod(cubemapSampler0, vout.FragLocalPos.xyz, 1.2).rgb;

  envColor = envColor / (envColor + vec3(1.0));
  envColor = pow(envColor, vec3(1.0/2.2));

  FragColor = vec4(envColor, 1.0);
}
