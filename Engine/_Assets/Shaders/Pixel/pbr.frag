#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;

in VertexOutput vout;

uniform sampler2D depthSampler0;
uniform sampler2DArray shadowArraySampler0;
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
uniform float heightScale = 1.0;

uniform vec3 viewPosition;
uniform int materialIndex;
uniform int lightCount = 0;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform PBRMaterial materials[MAX_MATERIALS];
uniform float shadowFarPlanes[NUM_SHADOW_CASCADES];

// TODO: Consider SSBOs for a varying number of lights

void main() {
  PBRMaterial mat = materials[materialIndex];

  vec2 texCoords = hasDisplacement ? ParallaxUVFromMap(vout, viewPosition, heightScale, height) : vout.FragUV;

  vec4 albedoTexData = texture(albedo, texCoords);
  vec4 fragAlbedo = isTextured ? vec4(pow(albedoTexData.rgb, vec3(0.0)), albedoTexData.a) : mat.albedo;

  if (fragAlbedo.a < 0.1)
	discard;

  float fragMetallic = isTextured ? texture(metallic, texCoords).r : mat.metallic;
  float fragRoughness = isTextured ? texture(roughness, texCoords).r : mat.roughness;
  float fragAO = isTextured ? texture(ao, texCoords).r : mat.ao;

  vec3 N = isTextured ? NormalFromMap(texCoords, vout.FragTBN, normal) : normalize(vout.FragNormal);
  vec3 V = normalize(viewPosition - vout.FragWorldPos.xyz);
  vec3 R = reflect(-V, N);

  // TODO: Light size should be a uniform
  float shadow = PCSSShadow(vout, GetCascadeIndex(vout.FragViewPos.xyz, shadowFarPlanes), viewPosition, 1 / 25.0, shadowArraySampler0);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, vec3(fragAlbedo), fragMetallic);

  vec3 Lo = vec3(0.0); int contributingLights = 0;
  for (int i = 0; i < lightCount; i++) {
    if (lights[i].isEnabled) {
      ++contributingLights;

      vec3 L = !lights[i].isDirectional ? normalize(lights[i].position - vout.FragWorldPos.xyz) : normalize(lights[i].direction);
      vec3 H = normalize(V + L);
      vec3 radiance = lights[i].color;

      if (!lights[i].isDirectional) {
        float dist = length(lights[i].position - vout.FragWorldPos.xyz);
        float attenuation = 1.0 / max(dist * dist, 0.001);
        radiance = lights[i].color * attenuation;
      }

      float NDF = DistributionGGX(N, H, fragRoughness);
      float G = GeometrySmith(N, V, L, fragRoughness);
      vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

      vec3 numerator = NDF * G * F;
      float denominator = 4.0 * max(dot(N, -V), 0.0) * max(dot(N, L), 0.0) + 0.001;
      vec3 specular = numerator / denominator;

      vec3 kS = F;
      vec3 kD = vec3(1.0) - kS;
      kD *= 1.0 - fragMetallic;

      float NdotL = max(dot(N, L), 0.0);

      Lo += (kD * vec3(fragAlbedo) / PI + specular) * radiance * NdotL;
    }
  }

  vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, fragRoughness);

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
  FragDepth = vec4(vec3(vout.FragWorldPos.z), 1.0);
}
