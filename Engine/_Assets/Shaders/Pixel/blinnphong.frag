#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/material.glsl" //! #include "../Uniforms/material.glsl"
#include "Shaders/Uniforms/light.glsl" //! #include "../Uniforms/light.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform sampler2D albedoSampler0;
uniform sampler2D depthSampler0;
uniform sampler2DArray shadowArraySampler0;

// TODO: Consider SSBOs for a varying number of lights

void main() {
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);
  vec3 ambient = vec3(0.0);

  vec3 norm = normalize(vout.FragNormal);
  vec3 viewDir = normalize(ViewPosition - vout.FragWorldPos.xyz);

  vec4 albd = material.albedo;
  if (isTextured)
  {
    vec4 albedoTexData = texture(albedoSampler0, vout.FragUV);
    albd = vec4(pow(albedoTexData.rgb, vec3(2.2)), albedoTexData.a);
  }

  if (albd.a < 0.1)
  {
    discard;
  }

  int cascadeIndex = GetCascadeIndex(vout.FragViewPos.xyz, shadowFarPlanes);
  float shadow = 0.0;

  if (light.isEnabled)
  {
      vec3 lightDir = light.lightType == LIGHT_TYPE_DIRECTIONAL ? normalize(light.direction.xyz) : normalize(light.position.xyz - vout.FragWorldPos.xyz);
      vec3 halfVector = normalize(lightDir + viewDir);
      float attenunation = 1.0;

      if (light.lightType != LIGHT_TYPE_DIRECTIONAL)
      {
          float lightDistance = length(lightDir);
          lightDir = lightDir / lightDistance;

          attenunation = 1.0 / (light.constantAttenuation
                              + light.linearAttenuation * lightDistance
                              + light.quadraticAttenuation * lightDistance * lightDistance);

          if (light.lightType == LIGHT_TYPE_SPOT)
          {
            float spotCos = dot(lightDir, -light.coneDirection.xyz);
            if (spotCos < light.spotCutoff)
            {
                attenunation = 0.0;
            }
            else
            {
                attenunation *= pow(spotCos, light.spotExponent);
            }
          }
      }

      float diff = max(0.0, dot(norm, lightDir));
      float spec = pow(max(0.0, dot(norm, halfVector)), material.shininess);

      shadow += PCFShadow(vout, cascadeIndex, lightDir, shadowArraySampler0);
      ambient += material.ambient * albd.rgb;
      diffuse += light.color.rgb * material.diffuse * albd.rgb * diff * attenunation;
      specular += light.color.rgb * material.specular * spec * attenunation;
  }

  vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) + material.emission;

  FragColor = vec4(color, albd.a);
}
