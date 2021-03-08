#include "Shaders/common.glsl" //! #include "../common.glsl"
#include "Shaders/Uniforms/post.glsl" //! #include "../Uniforms/post.glsl"
#include "Shaders/Uniforms/camera.glsl" //! #include "../Uniforms/camera.glsl"

out vec4 FragColor;

in VertexOutput vout;

uniform sampler2D colorSampler0;
uniform sampler2D depthSampler0;
uniform sampler2DArray shadowArraySampler0;

const int NUM_VELOCITY_SAMPLES = 8;

void main() {
	vec2 texCoord = vout.FragUV;
	vec4 color = texture(colorSampler0, texCoord);
	if (useMotionBlur)
	{
		// Extract world position from depth buffer
		float zOverW = texture(depthSampler0, texCoord).r;
		vec4 H = vec4(texCoord.x * 2.0 - 1.0, (1.0 - texCoord.y) * 2.0 - 1.0, zOverW, 1.0);
		vec4 D = InverseViewProjection * H;
		vec4 worldPos = D / D.w;

		// Compute velocity vector
		vec4 currentPos = H;
		vec4 previousPos = PreviousViewProjection * worldPos;
		previousPos /= previousPos.w;
		vec2 velocity = ((currentPos - previousPos) / 2.0).xy;

		// Sample the color sampler using the velocity vector to achieve motion blur
		texCoord -= velocity;
		for (int i = 0; i < NUM_VELOCITY_SAMPLES; ++i, texCoord -= (velocity * velocity * NUM_VELOCITY_SAMPLES * 0.5)) {
			vec4 currentColor = texture(colorSampler0, texCoord) * 0.5;
			color += currentColor;
		}
		color /= NUM_VELOCITY_SAMPLES;
	}
	
	FragColor = color;
}
