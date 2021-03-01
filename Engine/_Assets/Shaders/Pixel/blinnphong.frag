#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform int lightCount = 0;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];
uniform float shadowFarPlanes[NUM_SHADOW_CASCADES];
uniform vec3 viewPosition;
uniform int materialIndex;
uniform bool isTextured;
uniform sampler2D albedo;
uniform sampler2D depthSampler0;
uniform sampler2DArray shadowArraySampler0;

// TODO: Consider SSBOs for a varying number of lights

void main() {
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);
  vec3 ambient = vec3(0.0);

  vec3 norm = normalize(vout.FragNormal);
  vec3 viewDir = normalize(viewPosition - vout.FragWorldPos.xyz);

  vec4 albd = materials[materialIndex].albedo;
  if (isTextured)
  {
    vec4 albedoTexData = texture(albedo, vout.FragUV);
    albd = vec4(pow(albedoTexData.rgb, vec3(2.2)), albedoTexData.a);
  }
  if (albd.a < 0.01)
  {
    discard;
  }

  int cascadeIndex = GetCascadeIndex(vout.FragViewPos.xyz, shadowFarPlanes);

  // Regular Blinn-Phong over multiple lights, if present and enabled
  for (int i = 0; i < lightCount; i++) {
    if (!lights[i].isEnabled) continue;

    vec3 lightDir = lights[i].isDirectional ? normalize(lights[i].direction) : normalize(lights[i].position - vout.FragWorldPos.xyz);
    vec3 halfVector = normalize(lightDir + viewDir);
    float attenunation = 1.0;

    if (!lights[i].isDirectional) {
      float lightDistance = length(lightDir);
      lightDir = lightDir / lightDistance;

      attenunation = 1.0 / (lights[i].constantAttenuation
                            + lights[i].linearAttenuation * lightDistance
                            + lights[i].quadraticAttenuation * lightDistance * lightDistance);

      if (lights[i].isSpot) {
        float spotCos = dot(lightDir, -lights[i].coneDirection);
        if (spotCos < lights[i].spotCutoff) attenunation = 0.0;
        else attenunation *= pow(spotCos, lights[i].spotExponent);
      }
    }

    float diff = max(0.0, dot(norm, lightDir));
    float spec = pow(max(0.0, dot(norm, halfVector)), materials[materialIndex].shininess);

    float shadow = PCFShadow(vout, cascadeIndex, lightDir, shadowArraySampler0);

    ambient += materials[materialIndex].ambient * albd.rgb;
    diffuse += (1.0 - shadow) * lights[i].color * materials[materialIndex].diffuse * albd.rgb * diff * attenunation;
    specular += (1.0 - shadow) * lights[i].color * materials[materialIndex].specular * spec * attenunation;
  }

  vec3 color = (ambient + diffuse + specular) + materials[materialIndex].emission;

  FragColor = vec4(color, albd.a);
}
