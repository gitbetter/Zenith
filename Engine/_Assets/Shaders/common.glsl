const int MAX_LOCAL_LIGHTS = 4;
const int MAX_MATERIALS = 4;
const int MAX_BONES = 100;
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
  float emission;
  float ambient;
  float diffuse;
  float specular;
  float shininess;
};

struct PBRMaterial {
  vec4 albedo;
  float metallic;
  float roughness;
  float ao;
};

struct VertexOutput {
  vec4 FragLocalPos;
  vec4 FragPos;
  vec4 FragPosLightSpace;
  vec3 FragNormal;
  vec2 FragUV;
  mat3 FragTBN;
};

float Random(vec4 co) {
    return fract(sin(dot(co, vec4(12.9898, 78.233, 45.164, 94.673))) * 43758.5453);
}

float Random(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);

  return a2 / (PI * denom * denom);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float RadicalInverse_VdC(uint bits) {
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N) {
  return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
  float a = roughness * roughness;

  float phi = 2.0 * PI * Xi.x;
  float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
  float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

  vec3 H;
  H.x = cos(phi) * sinTheta;
  H.y = sin(phi) * sinTheta;
  H.z = cosTheta;

  vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent = normalize(cross(up, N));
  vec3 bitangent = cross(N, tangent);

  vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sampleVec);
}

float PCFShadow(VertexOutput vout, sampler2D shadowMap) {
  vec3 projCoords = vout.FragPosLightSpace.xyz / vout.FragPosLightSpace.w;
  if (projCoords.z > 1.0)
    return 0.0;

  projCoords = projCoords * 0.5 + 0.5;

  // PCF
  float shadow = 0.0;
  float bias = max(0.05 * (1.0 - dot(vout.FragNormal, vout.FragPosLightSpace.xyz - vout.FragPos.xzy)), 0.005);  
  for (int i = 0; i < 16; ++i) {
    float z = texture(shadowMap, projCoords.xy + poissonDisk[i]).r;
    shadow += z < (projCoords.z - bias) ? 1.0 : 0.0;
  }
  return shadow / 16.0;
}

float GetBlockerDistance(vec3 shadowCoords, vec3 viewPosition, float lightSize, sampler2D shadowMap) {
    int blockers = 0;
    float averageBlockerDistance = 0.0;
    float searchWidth = lightSize * (shadowCoords.z - 0.1) / viewPosition.z;
    for (int i = 0; i < 16; i++) {
      float z = texture(shadowMap, shadowCoords.xy + poissonDisk[i] * searchWidth).r;
      if (z < (shadowCoords.z - 0.005)) {
        ++blockers;
        averageBlockerDistance += z;
      }
    }

    if (blockers > 0) return averageBlockerDistance / blockers;
    else return -1;
}

float PCSSShadow(VertexOutput vout, vec3 viewPosition, float lightSize, sampler2D shadowMap) {
    vec3 projCoords = vout.FragPosLightSpace.xyz / vout.FragPosLightSpace.w;
    if (projCoords.z > 1.0)
      return 0.0;

    projCoords = projCoords * 0.5 + 0.5;

    float blockerDistance = GetBlockerDistance(projCoords, viewPosition, lightSize, shadowMap);
    if (blockerDistance == -1)
      return 0.0;

    float penumbraWidth = (projCoords.z - blockerDistance) / blockerDistance;

    float radius = penumbraWidth * lightSize * 0.1 / projCoords.z;

    // PCF
	float shadow = 0.0;
	float theta = Random(vec4(projCoords.xy, vout.FragPos.xy));
	mat2 rotation = mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));
	float bias = max(0.05 * (1.0 - dot(vout.FragNormal, vout.FragPos.xyz - vout.FragPosLightSpace.xyz)), 0.002);  
    for (int i = 0; i < 32; i++) {
	  vec2 offset = (rotation * poissonDisk[i]) * radius;
      float z = texture(shadowMap, projCoords.xy + offset).r;
      shadow += z < (projCoords.z - bias) ? 1.0 : 0.0;
    }
    return shadow / 32 * 0.85;
}

vec2 ParallaxUVFromMap(VertexOutput vout, vec3 viewPosition, float heightScale, sampler2D heightMap) {
  mat3 TBN = transpose(vout.FragTBN);
  vec3 viewDir = normalize(TBN * viewPosition - TBN * vout.FragPos.xyz);

  const float minLayers = 16.0;
  const float maxLayers = 128.0;
  float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

  float layerDepth = 1.0 / numLayers;
  float currentLayerDepth = 0.0;
  vec2 P = viewDir.xy / viewDir.z * heightScale;
  vec2 deltaTexCoords = P / numLayers;

  vec2 currentTexCoords = vout.FragUV;
  float currentDepthMapValue = texture(heightMap, currentTexCoords).r;

  while (currentLayerDepth < currentDepthMapValue) {
      currentTexCoords -= deltaTexCoords;
      currentDepthMapValue = texture(heightMap, currentTexCoords).r;
      currentLayerDepth += layerDepth;
  }

  vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
  float afterDepth = currentDepthMapValue - currentLayerDepth;
  float beforeDepth = (1.0 - texture(heightMap, prevTexCoords).r) - currentLayerDepth + layerDepth;

  float weight = afterDepth / (afterDepth - beforeDepth);
  vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

  return finalTexCoords;
}

vec3 NormalFromMap(vec2 uv, mat3 tbn, sampler2D normalMap) {
  vec3 tangentNormal = texture(normalMap, uv).xyz * 2.0 - 1.0;
  return normalize(tbn * tangentNormal);
}