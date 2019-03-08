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
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform sampler2D shadowMap;

uniform bool isTextured;

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
vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness);
float CalculateShadow(vec4 lightSpacePosition);
vec3 NormalFromMap();

void main() {
  Material mat = materials[materialIndex];

  vec4 fragAlbedo = isTextured ? vec4(pow(texture(albedo, fs_in.FragUV).rgb, vec3(2.2)), 1.0) : mat.albedo;
  float fragMetallic = isTextured ? texture(metallic, fs_in.FragUV).r : mat.metallic;
  float fragRoughness = isTextured ? texture(roughness, fs_in.FragUV).r : mat.roughness;
  float fragAO = isTextured ? texture(ao, fs_in.FragUV).r : mat.ao;

  vec3 N = isTextured ? NormalFromMap() : normalize(fs_in.FragNormal);
  vec3 V = normalize(viewDirection - fs_in.FragPos);
  vec3 R = reflect(V, N);
  float shadow = CalculateShadow(fs_in.FragPosLightSpace);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, vec3(fragAlbedo), fragMetallic);

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

      float NDF = DistributionGGX(N, H, fragRoughness);
      float G = GeometrySmith(N, V, L, fragRoughness);
      vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0, 0.0);

      vec3 numerator = NDF * G * F;
      float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
      vec3 specular = numerator / max(denominator, 0.001);

      vec3 kS = F;
      vec3 kD = vec3(1.0) - kS;
      kD *= 1.0 - fragMetallic;

      float NdotL = max(dot(N, L), 0.0);

      Lo += (kD * vec3(fragAlbedo) / PI + specular) * radiance * NdotL;
    }
  }

  Lo /= contributingLights;

  vec3 F = FresnelSchlick(max(dot(N, V), 0.0), F0, fragRoughness);

  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - fragMetallic;

  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = irradiance * vec3(fragAlbedo);

  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = textureLod(prefilterMap, R, fragRoughness * MAX_REFLECTION_LOD).rgb;
  vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), fragRoughness)).rg;
  vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * fragAO;

  vec3 color = ambient + (1.0 - shadow) * Lo;

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));

  FragColor = vec4(color, fragAlbedo.a);
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

vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
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
  return shadow / 12.0;
}

vec3 NormalFromMap() {
  vec3 tangentNormal = texture(normal, fs_in.FragUV).xyz * 2.0 - 1.0;

  vec3 Q1  = dFdx(fs_in.FragPos);
  vec3 Q2  = dFdy(fs_in.FragPos);
  vec2 st1 = dFdx(fs_in.FragUV);
  vec2 st2 = dFdy(fs_in.FragUV);

  vec3 N   = normalize(fs_in.FragNormal);
  vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B  = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}
