#version 460 core

const int MAX_LOCAL_LIGHTS = 25;
const int MAX_MATERIALS = 25;

const float PI = 3.14159265359;

vec2 poissonDisk[64] = vec2[]( 
	vec2( -0.04117257f, -0.1597612f ),
	vec2( 0.06731031f, -0.4353096f ),
	vec2( -0.206701f, -0.4089882f ),
	vec2( 0.1857469f, -0.2327659f ),
	vec2( -0.2757695f, -0.159873f ),
	vec2( -0.2301117f, 0.1232693f ),
	vec2( 0.05028719f, 0.1034883f ),
	vec2( 0.236303f, 0.03379251f ),
	vec2( 0.1467563f, 0.364028f ),
	vec2( 0.516759f, 0.2052845f ),
	vec2( 0.2962668f, 0.2430771f ),
	vec2( 0.3650614f, -0.1689287f ),
	vec2( 0.5764466f, -0.07092822f ),
	vec2( -0.5563748f, -0.4662297f ),
	vec2( -0.3765517f, -0.5552908f ),
	vec2( -0.4642121f, -0.157941f ),
	vec2( -0.2322291f, -0.7013807f ),
	vec2( -0.05415121f, -0.6379291f ),
	vec2( -0.7140947f, -0.6341782f ),
	vec2( -0.4819134f, -0.7250231f ),
	vec2( -0.7627537f, -0.3445934f ),
	vec2( -0.7032605f, -0.13733f ),
	vec2( 0.8593938f, 0.3171682f ),
	vec2( 0.5223953f, 0.5575764f ),
	vec2( 0.7710021f, 0.1543127f ),
	vec2( 0.6919019f, 0.4536686f ),
	vec2( 0.3192437f, 0.4512939f ),
	vec2( 0.1861187f, 0.595188f ),
	vec2( 0.6516209f, -0.3997115f ),
	vec2( 0.8065675f, -0.1330092f ),
	vec2( 0.3163648f, 0.7357415f ),
	vec2( 0.5485036f, 0.8288581f ),
	vec2( -0.2023022f, -0.9551743f ),
	vec2( 0.165668f, -0.6428169f ),
	vec2( 0.2866438f, -0.5012833f ),
	vec2( -0.5582264f, 0.2904861f ),
	vec2( -0.2522391f, 0.401359f ),
	vec2( -0.428396f, 0.1072979f ),
	vec2( -0.06261792f, 0.3012581f ),
	vec2( 0.08908027f, -0.8632499f ),
	vec2( 0.9636437f, 0.05915006f ),
	vec2( 0.8639213f, -0.309005f ),
	vec2( -0.03422072f, 0.6843638f ),
	vec2( -0.3734946f, -0.8823979f ),
	vec2( -0.3939881f, 0.6955767f ),
	vec2( -0.4499089f, 0.4563405f ),
	vec2( 0.07500362f, 0.9114207f ),
	vec2( -0.9658601f, -0.1423837f ),
	vec2( -0.7199838f, 0.4981934f ),
	vec2( -0.8982374f, 0.2422346f ),
	vec2( -0.8048639f, 0.01885651f ),
	vec2( -0.8975322f, 0.4377489f ),
	vec2( -0.7135055f, 0.1895568f ),
	vec2( 0.4507209f, -0.3764598f ),
	vec2( -0.395958f, -0.3309633f ),
	vec2( -0.6084799f, 0.02532744f ),
	vec2( -0.2037191f, 0.5817568f ),
	vec2( 0.4493394f, -0.6441184f ),
	vec2( 0.3147424f, -0.7852007f ),
	vec2( -0.5738106f, 0.6372389f ),
	vec2( 0.5161195f, -0.8321754f ),
	vec2( 0.6553722f, -0.6201068f ),
	vec2( -0.2554315f, 0.8326268f ),
	vec2( -0.5080366f, 0.8539945f )
);

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;

in VS_OUT {
  vec3 FragPos;
  vec3 FragNormal;
  vec2 FragUV;
  mat3 FragTBN;
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
  float alpha;
  float metallic;
  float roughness;
  float ao;
};

uniform sampler2D depthTexture;
uniform sampler2D shadowTexture;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform sampler1D poissonDistribution;

uniform bool isTextured;
uniform bool hasDisplacement;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D metallic;
uniform sampler2D roughness;
uniform sampler2D ao;
uniform sampler2D height;
uniform float heightScale;

uniform vec3 viewPosition;
uniform int materialIndex;
uniform int lightCount = 0;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];
uniform HemisphereLight hemisphereLight;

// TODO: Consider SSBOs for a varying number of lights

// PBR Calculations
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness);

// Shadow Calculations
float PCFShadow(vec4 lightSpacePosition);
float GetBlockerDistance(vec3 shadowCoords, float lightSize);
float PCSSShadow(vec4 lightSpacePosition, float lightSize);

// UV and Normal Calculations
vec2 ParallaxFromMap();
vec3 NormalFromMap(vec2 uv);

// Helpers
float Random(vec4 co);

void main() {
  Material mat = materials[materialIndex];

  vec2 texCoords = hasDisplacement ? ParallaxFromMap() : fs_in.FragUV;

  vec4 albedoTexData = texture(albedo, texCoords);
  vec4 fragAlbedo = isTextured ? vec4(pow(albedoTexData.rgb, vec3(2.2)), albedoTexData.a) : mat.albedo;

  if (fragAlbedo.a < 0.1)
	discard;

  float fragMetallic = isTextured ? texture(metallic, texCoords).r : mat.metallic;
  float fragRoughness = isTextured ? texture(roughness, texCoords).r : mat.roughness;
  float fragAO = isTextured ? texture(ao, texCoords).r : mat.ao;

  vec3 N = isTextured ? NormalFromMap(texCoords) : normalize(fs_in.FragNormal);
  vec3 V = normalize(viewPosition - fs_in.FragPos);
  vec3 R = reflect(-V, N);
  float shadow = PCSSShadow(fs_in.FragPosLightSpace, 1 / 25.0);	// TODO: Light size should be a uniform

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, vec3(fragAlbedo), fragMetallic);

  vec3 Lo = vec3(0.0); int contributingLights = 0;
  for (int i = 0; i < lightCount; i++) {
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
      float denominator = 4.0 * max(dot(N, -V), 0.0) * max(dot(N, L), 0.0);
      vec3 specular = numerator / max(denominator, 0.001);

      vec3 kS = F;
      vec3 kD = vec3(1.0) - kS;
      kD *= 1.0 - fragMetallic;

      float NdotL = max(dot(N, L), 0.0);

      Lo += (kD * vec3(fragAlbedo) / PI + specular) * radiance * NdotL;
    }
  }

  Lo /= float(contributingLights);

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
  FragDepth = vec4(vec3(fs_in.FragPos.z), 1.0);
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

float PCFShadow(vec4 lightSpacePosition) {
  vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
  if (projCoords.z > 1.0)
    return 0.0;

  projCoords = projCoords * 0.5 + 0.5;

  // PCF
  float shadow = 0.0;
  float bias = max(0.05 * (1.0 - dot(fs_in.FragNormal, lightSpacePosition.xyz - fs_in.FragPos)), 0.005);  
  for (int i = 0; i < 16; ++i) {
    float z = texture(shadowTexture, projCoords.xy + poissonDisk[i]).r;
    shadow += z < (projCoords.z - bias) ? 1.0 : 0.0;
  }
  return shadow / 16.0;
}

float GetBlockerDistance(vec3 shadowCoords, float lightSize) {
    int blockers = 0;
    float averageBlockerDistance = 0.0;
    float searchWidth = lightSize * (shadowCoords.z - 0.1) / viewPosition.z;
    for (int i = 0; i < 16; i++) {
      float z = texture(shadowTexture, shadowCoords.xy + poissonDisk[i] * searchWidth).r;
      if (z < (shadowCoords.z - 0.005)) {
        ++blockers;
        averageBlockerDistance += z;
      }
    }

    if (blockers > 0) return averageBlockerDistance / blockers;
    else return -1;
}

float PCSSShadow(vec4 lightSpacePosition, float lightSize) {
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    if (projCoords.z > 1.0)
      return 0.0;

    projCoords = projCoords * 0.5 + 0.5;

    float blockerDistance = GetBlockerDistance(projCoords, lightSize);
    if (blockerDistance == -1)
      return 0.0;

    float penumbraWidth = (projCoords.z - blockerDistance) / blockerDistance;

    float radius = penumbraWidth * lightSize * 0.1 / projCoords.z;

    // PCF
	float shadow = 0.0;
	float theta = Random(vec4(projCoords.xy, fs_in.FragPos.xy));
	mat2 rotation = mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));
	float bias = max(0.05 * (1.0 - dot(fs_in.FragNormal, fs_in.FragPos - lightSpacePosition.xyz)), 0.002);  
    for (int i = 0; i < 32; i++) {
	  vec2 offset = (rotation * poissonDisk[i]) * radius;
      float z = texture(shadowTexture, projCoords.xy + offset).r;
      shadow += z < (projCoords.z - bias) ? 1.0 : 0.0;
    }
    return shadow / 32 * 0.85;
}

vec2 ParallaxFromMap() {
  mat3 TBN = transpose(fs_in.FragTBN);
  vec3 viewDir = normalize(TBN * viewPosition - TBN * fs_in.FragPos);

  const float minLayers = 16.0;
  const float maxLayers = 128.0;
  float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

  float layerDepth = 1.0 / numLayers;
  float currentLayerDepth = 0.0;
  vec2 P = viewDir.xy / viewDir.z * heightScale;
  vec2 deltaTexCoords = P / numLayers;

  vec2 currentTexCoords = fs_in.FragUV;
  float currentDepthMapValue = texture(height, currentTexCoords).r;

  while (currentLayerDepth < currentDepthMapValue) {
      currentTexCoords -= deltaTexCoords;
      currentDepthMapValue = texture(height, currentTexCoords).r;
      currentLayerDepth += layerDepth;
  }

  vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
  float afterDepth = currentDepthMapValue - currentLayerDepth;
  float beforeDepth = (1.0 - texture(height, prevTexCoords).r) - currentLayerDepth + layerDepth;

  float weight = afterDepth / (afterDepth - beforeDepth);
  vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

  return finalTexCoords;
}

vec3 NormalFromMap(vec2 uv) {
  vec3 tangentNormal = texture(normal, uv).xyz * 2.0 - 1.0;
  return normalize(fs_in.FragTBN * tangentNormal);
}

float Random(vec4 co) {
  float dp = dot(co, vec4(12.9898, 78.233, 45.164, 94.673));
  return fract(sin(dp) * 43758.5453);
}
