#version 330 core

in vec3 FragPos;
in vec3 FragNormal;
in vec3 FragUV;

out vec4 FragColor;

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

uniform vec3 viewDirection;
uniform int materialIndex;

const int MAX_LOCAL_LIGHTS = 15;
const int MAX_MATERIALS = 15;
uniform Light lights[MAX_LOCAL_LIGHTS];
uniform Material materials[MAX_MATERIALS];
// TODO? Consider SSBOs for a varying number of lights

void main() {
  vec3 scatteredLight = vec3(0.0);
  vec3 reflectedLight = vec3(0.0);

  for (int i = 0; i < MAX_LOCAL_LIGHTS; i++) {
    if (!lights[i].isEnabled) continue;

    vec3 lightDirection = lights[i].direction;
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float attenunation = 1.0;

    if (!lights[i].isDirectional) {
      vec3 lightDirection = lightDirection - FragPos;
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

    float diffuse = max(0.0, dot(FragNormal, lightDirection));
    float specular = max(0.0, dot(FragNormal, halfVector));

    if (diffuse == 0.0) specular = 0.0;
    else specular = pow(specular, materials[materialIndex].shininess);

    scatteredLight += lights[i].ambient * materials[materialIndex].ambient * attenunation +
                      lights[i].color * materials[materialIndex].diffuse * diffuse * attenunation;
    reflectedLight += lights[i].color * materials[materialIndex].specular * specular * attenunation;
  }

  vec3 color = min(materials[materialIndex].emission + materials[materialIndex].albedo.rgb * scatteredLight + reflectedLight, vec3(1.0));
  FragColor = vec4(color, materials[materialIndex].albedo.a);
}
