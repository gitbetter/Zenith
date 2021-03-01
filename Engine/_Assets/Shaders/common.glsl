#version 450 core

#include "Shaders/constants.glsl" //! #include "constants.glsl"

/************* Shared Structs ***************/

struct VertexOutput {
  vec4 FragLocalPos;
  vec4 FragWorldPos;
  vec4 FragViewPos;
  vec3 FragNormal;
  vec2 FragUV;
  mat3 FragTBN;
  vec4 FragPosLightSpace[NUM_SHADOW_CASCADES];
};

/************* Common Helper Functions ***************/

float Random(vec4 co) {
    return fract(sin(dot(co, vec4(12.9898, 78.233, 45.164, 94.673))) * 43758.5453);
}

float Random(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float LinearizeDepth(float depth, float near, float far) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

/************* PBR Helper Functions ***************/

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

/************* Shadow Mapping Helper Functions ***************/

int GetCascadeIndex(vec3 position, vec4 farPlanes) {
    int index = 0;
    for (int i = 0; i < NUM_SHADOW_CASCADES; i++) {
        if (abs(position.z) < farPlanes[i]) {
            index = i; break;
        }
    }
    return index;
}

float PCFShadow(VertexOutput vout, int cascade, vec3 lightDir, sampler2DArray shadowMap) {
    // perform perspective divide
	vec3 projCoords = vout.FragPosLightSpace[cascade].xyz / vout.FragPosLightSpace[cascade].w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy, cascade)).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(vout.FragNormal);
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0002);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, cascade)).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float GetBlockerDistance(vec3 shadowCoords, int cascade, vec3 viewPosition, float lightSize, sampler2DArray shadowMap) {
    int blockers = 0;
    float averageBlockerDistance = 0.0;
    float searchWidth = lightSize * (shadowCoords.z - 0.1) / viewPosition.z;
    for (int i = 0; i < 16; i++) {
      float z = texture(shadowMap, vec3(shadowCoords.xy + poissonDisk[i] * searchWidth, cascade)).r;
      if (z < (shadowCoords.z - 0.002)) {
        ++blockers;
        averageBlockerDistance += z;
      }
    }

    if (blockers > 0) return averageBlockerDistance / blockers;
    else return -1;
}

float PCSSShadow(VertexOutput vout, int cascade, vec3 viewPosition, float lightSize, sampler2DArray shadowMap) {
    vec3 projCoords = vout.FragPosLightSpace[cascade].xyz / vout.FragPosLightSpace[cascade].w;

    projCoords = projCoords * 0.5 + 0.5;

    float blockerDistance = GetBlockerDistance(projCoords, cascade, viewPosition, lightSize, shadowMap);
    if (blockerDistance == -1)
      return 0.0;

    float penumbraWidth = (projCoords.z - blockerDistance) / blockerDistance;

    float radius = penumbraWidth * lightSize * 0.1 / projCoords.z;

    // PCF
	float shadow = 0.0;
	float theta = Random(vec4(projCoords.xy, vout.FragWorldPos.xy));
	mat2 rotation = mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));
	float bias = max(0.002 * (1.0 - dot(vout.FragNormal, viewPosition)), 0.0002);  
    for (int i = 0; i < 16; i++) {
	  vec2 offset = (rotation * poissonDisk[i]) * radius;
      float pcssDepth = texture(shadowMap, vec3(projCoords.xy + offset, cascade)).r;
      shadow += projCoords.z - bias > pcssDepth ? 1.0 : 0.0;
    }

	if (projCoords.z > 1.0)
      return 0.0;

    return shadow / 16 * 0.85;
}

/************* Normal Mapping Helper Functions ***************/

vec2 ParallaxUVFromMap(VertexOutput vout, vec3 viewPosition, float heightScale, sampler2D heightMap) {
  mat3 TBN = transpose(vout.FragTBN);
  vec3 viewDir = normalize(TBN * viewPosition - TBN * vout.FragWorldPos.xyz);

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

/****************************************************************/