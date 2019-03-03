#version 400 core

const int MAX_LOCAL_LIGHTS = 15;
const int MAX_MATERIALS = 15;
const float PI = 3.14159265359;

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

uniform samplerCube irradianceMap;
uniform sampler2D shadowMap;
uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D metallic;
uniform sampler2D roughness;
uniform sampler2D ao;

uniform vec3 viewDirection;
uniform int materialIndex;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];
uniform HemisphereLight hemisphereLight;

// TODO: Consider SSBOs for a varying number of lights

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
float CalculateShadow(vec4 lightSpacePosition);

void main() {
  Material mat = materials[materialIndex];
  vec3 N = normalize(fs_in.FragNormal);
  vec3 V = normalize(viewDirection - fs_in.FragPos);
  float shadow = CalculateShadow(fs_in.FragPosLightSpace);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, vec3(mat.albedo), mat.metallic);

  vec3 Lo = vec3(0.0); int contributingLights = 0;
  for (int i = 0; i < MAX_LOCAL_LIGHTS; i++) {
    if (lights[i].isEnabled) {
      ++contributingLights;
      vec3 L = !lights[i].isDirectional ? normalize(lights[i].position - fs_in.FragPos) : normalize(lights[i].direction);
      vec3 H = normalize(V + L);
      vec3 radiance = lights[i].color;

      if (!lights[i].isDirectional) {
        float dist = length(lights[i].position - fs_in.FragPos);
        float attenuation = 1.0 / max(dist * dist, 0.001);
        radiance = lights[i].color * attenuation;
      }

      float NDF = DistributionGGX(N, H, mat.roughness);
      float G = GeometrySmith(N, V, L, mat.roughness);
      vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

      vec3 numerator = NDF * G * F;
      float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
      vec3 specular = numerator / max(denominator, 0.001);

      vec3 kS = F;
      vec3 kD = vec3(1.0) - kS;
      kD *= 1.0 - mat.metallic;

      float NdotL = max(dot(N, L), 0.0);

      Lo += (kD * vec3(mat.albedo) / PI + specular) * radiance * NdotL;
    }
  }

  Lo /= contributingLights;

  vec3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - mat.metallic;
  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = irradiance * vec3(mat.albedo);
  vec3 ambient = (kD * diffuse) * mat.ao;

  vec3 color = ambient + (1.0 - shadow) * Lo;

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));

  FragColor = vec4(color, mat.albedo.a);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float num = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float num = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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
      float bias = 0.005;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / 9.0;
}
