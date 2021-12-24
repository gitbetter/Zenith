#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/material.glsl" //! #include "../Uniforms/material.glsl"
#include "Shaders/Uniforms/light.glsl" //! #include "../Uniforms/light.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragDepth;

in VertexOutput vout;

uniform sampler2D depthSampler0;
uniform sampler2DArray shadowArraySampler0;
uniform samplerCube irradianceSampler0;
uniform samplerCube prefilterSampler0;
uniform sampler2D lutSampler0;

uniform sampler2D albedoSampler0;
uniform sampler2D normalSampler0;
uniform sampler2D metallicSampler0;
uniform sampler2D roughnessSampler0;
uniform sampler2D aoSampler0;
uniform sampler2D heightSampler0;
uniform float heightScale = 1.0;

// TODO: Consider SSBOs for a varying number of lights

void main() {
  vec2 texCoords = hasDisplacement ? ParallaxUVFromMap(vout, ViewPosition, heightScale, heightSampler0) : vout.FragUV;

  vec4 albedoTexData = texture(albedoSampler0, texCoords);
  vec4 fragAlbedo = isTextured ? vec4(pow(albedoTexData.rgb, vec3(0.0)), albedoTexData.a) : material.albedo;

  if (fragAlbedo.a < 0.1)
	discard;

  float fragMetallic = isTextured ? texture(metallicSampler0, texCoords).r : material.metallic;
  float fragRoughness = isTextured ? texture(roughnessSampler0, texCoords).r : material.roughness;
  float fragAO = isTextured ? texture(aoSampler0, texCoords).r : material.ao;

  vec3 N = isTextured ? NormalFromMap(texCoords, vout.FragTBN, normalSampler0) : normalize(vout.FragNormal);
  vec3 V = normalize(ViewPosition - vout.FragWorldPos.xyz);
  vec3 R = reflect(-V, N);

  // TODO: Light size should be a uniform
  float shadow = PCSSShadow(vout, GetCascadeIndex(vout.FragViewPos.xyz, shadowFarPlanes), ViewPosition, 1 / 25.0, shadowArraySampler0);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, vec3(fragAlbedo), fragMetallic);

  vec3 Lo = vec3(0.0); int contributingLights = 0;
  if (light.isEnabled) {
    ++contributingLights;

    vec3 L = light.lightType != LIGHT_TYPE_DIRECTIONAL ? normalize(light.position.xyz - vout.FragWorldPos.xyz) : normalize(light.direction.xyz);
    vec3 H = normalize(V + L);
    vec3 radiance = light.color.rgb;

    if (light.lightType != LIGHT_TYPE_DIRECTIONAL) {
    float dist = length(light.position.xyz - vout.FragWorldPos.xyz);
    float attenuation = 1.0 / max(dist * dist, 0.001);
    radiance = light.color.rgb * attenuation;
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

  vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, fragRoughness);

  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - fragMetallic;

  vec3 irradiance = texture(irradianceSampler0, N).rgb;
  vec3 diffuse = irradiance * vec3(fragAlbedo);

  const float MAX_REFLECTION_LOD = 4.0;
  vec3 prefilteredColor = textureLod(prefilterSampler0, R, fragRoughness * MAX_REFLECTION_LOD).rgb;
  vec2 brdf = texture(lutSampler0, vec2(max(dot(N, V), 0.0), fragRoughness)).rg;
  vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

  vec3 ambient = (kD * diffuse + specular) * fragAO;

  vec3 color = ambient + (1.0 - shadow) * Lo;

  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0/2.2));

  FragColor = vec4(color, fragAlbedo.a);
  FragDepth = vec4(vec3(vout.FragWorldPos.z), 1.0);
}
