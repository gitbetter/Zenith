//? #version 450 core
//? #include "../constants.glsl"

struct Light {
  vec4 ambient;
  vec4 color;
  vec4 position;
  vec4 direction;
  vec4 coneDirection;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;
  float spotCutoff;
  float spotExponent;
  bool isEnabled;
  uint lightType;
};

layout (std140, binding = 5) uniform Lights
{
    mat4 ViewProjectionLightSpace;
    mat4 ViewProjectionsLightSpace[NUM_SHADOW_CASCADES];
    vec4 shadowFarPlanes;
    Light light;
};