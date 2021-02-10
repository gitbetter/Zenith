#version 450 core

#include "Shaders/common.glsl" //! #include "../common.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform unsigned int lightCount = 0;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];
uniform vec3 viewPosition;
uniform int materialIndex;
uniform bool isTextured;
uniform sampler2D shadowSampler0;
uniform sampler2D albedo;

// TODO: Consider SSBOs for a varying number of lights

void main() {
  vec3 scatteredLight = vec3(0.0);
  vec3 reflectedLight = vec3(0.0);

  vec3 norm = normalize(vout.FragNormal);

  // Regular Blinn-Phong over multiple lights, if present and enabled
  for (int i = 0; i < lightCount; i++) {
    if (!lights[i].isEnabled) continue;

    vec3 lightDirection = lights[i].isDirectional ? normalize(lights[i].direction) : normalize(lights[i].position - vout.FragPos.xyz);
    vec3 halfVector = normalize(lightDirection + (viewPosition - vout.FragPos.xyz));
    float attenunation = 1.0;

    if (!lights[i].isDirectional) {
      float lightDistance = length(lightDirection);
      lightDirection = lightDirection / lightDistance;

      attenunation = 1.0 / (lights[i].constantAttenuation
                            + lights[i].linearAttenuation * lightDistance
                            + lights[i].quadraticAttenuation * lightDistance * lightDistance);

      if (lights[i].isSpot) {
        float spotCos = dot(lightDirection, -lights[i].coneDirection);
        if (spotCos < lights[i].spotCutoff) attenunation = 0.0;
        else attenunation *= pow(spotCos, lights[i].spotExponent);
      }
    }

    float diffuse = max(0.0, dot(norm, lightDirection));
    float specular = pow(max(0.0, dot(norm, halfVector)), materials[materialIndex].shininess);

    scatteredLight += lights[i].ambient * materials[materialIndex].ambient * attenunation +
                      lights[i].color * materials[materialIndex].diffuse * diffuse * attenunation;
    reflectedLight += lights[i].color * materials[materialIndex].specular * specular * attenunation;
  }

  vec4 albd = materials[materialIndex].albedo;
  if (isTextured)
  {
    vec4 albedoTexData = texture(albedo, vout.FragUV);
    albd = vec4(pow(albedoTexData.rgb, vec3(2.2)), albedoTexData.a);
  }
  if (albd.a < 0.1)
  {
    discard;
  }

  float shadow = PCFShadow(vout, shadowSampler0);
  vec3 color = min(materials[materialIndex].emission + albd.rgb + (1.0 - shadow) * (scatteredLight + reflectedLight), vec3(1.0));

  FragColor = vec4(color, albd.a);
}
