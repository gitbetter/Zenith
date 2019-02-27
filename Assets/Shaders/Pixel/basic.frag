#version 330 core

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
  float emission;
  float ambient;
  float diffuse;
  float specular;
  float shininess;
};

uniform sampler2D shadowMap;

uniform vec3 viewDirection;
uniform int materialIndex;  // Single index across the fragments of a single mesh

const int MAX_LOCAL_LIGHTS = 15;
const int MAX_MATERIALS = 15;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];

uniform HemisphereLight hemisphereLight;
// TODO: Consider SSBOs for a varying number of lights

float CalculateShadow(vec4 lightSpacePosition);

void main() {
  vec3 scatteredLight = vec3(0.0);
  vec3 reflectedLight = vec3(0.0);

  // Regular Blinn-Phong over multiple lights, if present and enabled
  for (int i = 0; i < MAX_LOCAL_LIGHTS; i++) {
    if (!lights[i].isEnabled) continue;

    vec3 lightDirection = lights[i].position;
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float attenunation = 1.0;

    if (!lights[i].isDirectional) {
      lightDirection = lightDirection - fs_in.FragPos;
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
    } else {
      halfVector = lights[i].direction;
    }

    float diffuse = max(0.0, dot(fs_in.FragNormal, lightDirection));
    float specular = max(0.0, dot(fs_in.FragNormal, halfVector));
    specular = (diffuse == 0.0) ? 0.0 : pow(specular, materials[materialIndex].shininess);

    scatteredLight += lights[i].ambient * materials[materialIndex].ambient * attenunation +
                      lights[i].color * materials[materialIndex].diffuse * diffuse * attenunation;
    reflectedLight += lights[i].color * materials[materialIndex].specular * specular * attenunation;
  }

  float shadow = CalculateShadow(fs_in.FragPosLightSpace);
  vec3 color = min(materials[materialIndex].emission + materials[materialIndex].albedo.rgb + (1.0 - shadow) * scatteredLight + reflectedLight, vec3(1.0));

  vec3 hemisphereLightDirection = normalize(hemisphereLight.position - fs_in.FragPos);
  float a = dot(fs_in.FragNormal, hemisphereLightDirection) * 0.5 + 0.5;
  color += mix(hemisphereLight.groundColor, hemisphereLight.skyColor, a);

  FragColor = vec4(color, materials[materialIndex].albedo.a);
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
      shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / 9.0;
}
