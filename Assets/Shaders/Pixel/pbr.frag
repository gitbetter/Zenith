#version 400 core

const int MAX_LOCAL_LIGHTS = 15;
const int MAX_MATERIALS = 15;

out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec3 FragNormal;
  vec2 FragUV;
  vec4 FragPosLightSpace;
} fs_in;

struct HemisphereLight {
  vec3 position;
  vec3 skyColor;
  vec3 groundColor;
};

struct Light {
  bool isEnabled;
  bool isSpot;
  bool isDirectional;
  vec3 ambient;
  vec3 color;
  vec3 position;
  vec3 direction;
  vec3 coneDirection;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;
  float spotCutoff;
  float spotExponent;
};

struct Material {
  vec4 albedo;
  float metallic;
  float roughness;
  float ao;
};

uniform sampler2D shadowMap;

uniform vec3 viewDirection;
uniform int materialIndex;  // Single index across the fragments of a single mesh
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];
uniform HemisphereLight hemisphereLight;

// TODO: Consider SSBOs for a varying number of lights

float CalculateShadow(vec4 lightSpacePosition);

void main() {

}

float CalculateShadow(vec4 lightSpacePosition) {
  vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
  if (projCoords.z > 1.0)
    return 0.0;
  projCoords = projCoords * 0.5 + 0.5;
  float currentDepth = projCoords.z;

  // Some PCF to soften out the shadows
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / 9.0;
}
