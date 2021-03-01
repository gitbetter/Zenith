//? #version 450 core

struct Material {
  vec4 albedo;
  float emission;
  float ambient;
  float diffuse;
  float specular;
  float shininess;
};

struct PBRMaterial {
  vec4 albedo;
  float metallic;
  float roughness;
  float ao;
};

layout (std140, binding = 4) uniform Materials
{
    Material material;
    PBRMaterial pbrMaterial;
    bool isTextured;
    bool hasDisplacement;
};