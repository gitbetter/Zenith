#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/material.glsl" //! #include "../Uniforms/material.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform sampler2D albedoSampler0;

// TODO: Consider SSBOs for a varying number of lights

void main() {
  vec4 albd = material.albedo;
  if (isTextured)
  {
    vec4 albedoTexData = texture(albedoSampler0, vout.FragUV);
    albd = vec4(pow(albedoTexData.rgb, vec3(2.2)), albedoTexData.a);
  }
  if (albd.a < 0.1)
  {
    discard;
  }

  FragColor = albd;
}
