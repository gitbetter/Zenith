#include "Shaders/common.glsl" //! #include "../../../../_Assets/Shaders/common.glsl"
#include "Shaders/Uniforms/ui.glsl" //! #include "../../../../_Assets/Shaders/Uniforms/ui.glsl"

in VertexOutput vout;

out vec4 FragColor;

float fill(float x, float size) {
    return 1.-smoothstep(size-.05, size+.05, x);
}

float triangleSDF(vec2 st) {
    st = (st * 2.-1.)*2.;
    return max(abs(st.x) * 0.866025 + st.y * 0.5, -st.y * 0.5);
}

void main() {
  vec4 fragColor = vec4(0.);
  vec2 st = vout.FragUV;
  st.y = 1.-st.y;
  fragColor += fill(triangleSDF(st), 0.35);
  fragColor *= color;
  FragColor = fragColor;
}