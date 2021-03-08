#include "Shaders/common.glsl" //! #include "../common.glsl"

out vec2 FragColor;

in VertexOutput vout;

vec2 IntegrateBRDF(float NdotV, float roughness);

void main() {
  vec2 IntegrateBRDF = IntegrateBRDF(vout.FragUV.x, vout.FragUV.y);
  FragColor = IntegrateBRDF;
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
  vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);

  float A = 0.0;
  float B = 0.0;

  vec3 N = vec3(0.0, 0.0, 1.0);

  const uint SAMPLE_COUNT = 1024u;
  for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    float VdotH = dot(V, H);
    vec3 L = normalize(2.0 * VdotH * H - V);

    float NdotL = max(L.z, 0.0);
    float NdotH = max(H.z, 0.0);
    VdotH = max(dot(V, H), 0.0);

    if (NdotL > 0.0) {
      float G = GeometrySmith(N, V, L, roughness);
      float G_Vis = (G * VdotH) / (NdotH * NdotV);
      float Fc = pow(1.0 - VdotH, 5.0);

      A += (1.0 - Fc) * G_Vis;
      B += Fc * G_Vis;
    }
  }
  A /= float(SAMPLE_COUNT);
  B /= float(SAMPLE_COUNT);
  return vec2(A, B);
}
