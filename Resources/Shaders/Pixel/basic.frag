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
  vec3 emission;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
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

    vec3 lightDirection = lights[i].direction;
    vec3 halfVector = normalize(lights[i].direction + viewDirection);
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

    if (diffuse == 0.0) specular = 0.0;
    else specular = pow(specular, materials[materialIndex].shininess);

    scatteredLight += lights[i].ambient * materials[materialIndex].ambient +
                      lights[i].color * materials[materialIndex].diffuse * diffuse * attenunation;
    reflectedLight += lights[i].color * materials[materialIndex].specular * specular * attenunation;
  }

  float shadow = CalculateShadow(fs_in.FragPosLightSpace);
  vec3 color = min(materials[materialIndex].emission + materials[materialIndex].albedo.rgb + (1.0 - shadow) * (scatteredLight + reflectedLight), vec3(1.0));

  // Hemisphere lighting
  vec3 hemisphereLightDirection = normalize(hemisphereLight.position - fs_in.FragPos);
  float a = dot(fs_in.FragNormal, hemisphereLightDirection) * 0.5 + 0.5;
  color += mix(hemisphereLight.groundColor, hemisphereLight.skyColor, a);

  FragColor = vec4(color, materials[materialIndex].albedo.a);
}

float CalculateShadow(vec4 lightSpacePosition) {
  vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  return (currentDepth > closestDepth) ? 1.0 : 0.0;
}
