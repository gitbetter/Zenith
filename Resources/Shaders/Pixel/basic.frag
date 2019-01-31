#version 330 core

in vec3 FragPos;
in vec3 FragNormal;
in vec3 FragUV;

out vec4 FragColor;

uniform vec4 baseColor;
uniform vec3 ambientLight;
uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform vec3 viewDirection;
uniform float shininess;
uniform float strength;

void main() {
  vec3 halfVector = normalize(lightDirection + viewDirection);

  float diffuse = max(0.0, dot(FragNormal, lightDirection));
  float specular = max(0.0, dot(FragNormal, halfVector));

  if (diffuse == 0.0) specular = 0.0;
  else specular = pow(specular, shininess);

  vec3 scatteredLight = ambientLight + lightColor * diffuse;
  vec3 reflectedLight = lightColor * specular * strength;

  vec3 color = min(baseColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
  FragColor = vec4(color, baseColor.a);
}
