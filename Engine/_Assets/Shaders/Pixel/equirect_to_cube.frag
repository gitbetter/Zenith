#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v) {
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= invAtan;
  uv += 0.5;
  return uv;
}

void main() {
  vec2 uv = SampleSphericalMap(normalize(vout.FragLocalPos.xyz));
  vec3 color = texture(equirectangularMap, uv).rgb;

  FragColor = vec4(color, 1.0);
}
