#version 450 core

const int MAX_LOCAL_LIGHTS = 4;
const int MAX_MATERIALS = 4;

out vec4 FragColor;

in VS_OUT {
  vec3 FragPos;
  vec3 FragNormal;
  vec2 FragUV;
  mat3 FragTBN;
  vec4 FragPosLightSpace;
} fs_in;

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

uniform unsigned int lightCount = 0;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];

uniform vec3 viewPosition;
uniform int materialIndex;

uniform bool isTextured;

uniform sampler2D shadowSampler0;
uniform sampler2D albedo;


// TODO: Consider SSBOs for a varying number of lights

float CalculateShadow(vec4 lightSpacePosition);

void main() {
  vec3 scatteredLight = vec3(0.0);
  vec3 reflectedLight = vec3(0.0);

  vec3 norm = normalize(fs_in.FragNormal);

  // Regular Blinn-Phong over multiple lights, if present and enabled
  for (int i = 0; i < lightCount; i++) {
    if (!lights[i].isEnabled) continue;

    vec3 lightDirection = lights[i].isDirectional ? normalize(lights[i].direction) : normalize(lights[i].position - fs_in.FragPos);
    vec3 halfVector = normalize(lightDirection + (viewPosition - fs_in.FragPos));
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
    vec4 albedoTexData = texture(albedo, fs_in.FragUV);
    albd = vec4(pow(albedoTexData.rgb, vec3(2.2)), albedoTexData.a);
  }
  if (albd.a < 0.1)
  {
    discard;
  }

  float shadow = CalculateShadow(fs_in.FragPosLightSpace);
  vec3 color = min(materials[materialIndex].emission + albd.rgb + (1.0 - shadow) * (scatteredLight + reflectedLight), vec3(1.0));

  FragColor = vec4(color, albd.a);
}

float CalculateShadow(vec4 lightSpacePosition) {
  vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
  if (projCoords.z > 1.0)
    return 0.0;
  projCoords = projCoords * 0.5 + 0.5;
  float currentDepth = projCoords.z;

  // Some PCF to soften out the shadows
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowSampler0, 0);
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(shadowSampler0, projCoords.xy + vec2(x, y) * texelSize).r;
      float bias = 0.005;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / 9.0;
}
