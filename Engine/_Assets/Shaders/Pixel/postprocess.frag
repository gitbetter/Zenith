#version 450 core

out vec4 FragColor;

in vec2 FragUV;

uniform mat4 previousViewProjection;
uniform mat4 inverseViewProjection;
uniform sampler2D colorSampler;
uniform sampler2D depthTexture;
uniform bool useMotionBlur;

const int NUM_VELOCITY_SAMPLES = 8;

void main() {
	vec2 texCoord = FragUV;
	vec4 color = texture(colorSampler, texCoord);
	if (useMotionBlur)
	{
		// Extract world position from depth buffer
		float zOverW = texture(depthTexture, texCoord).r;
		vec4 H = vec4(texCoord.x * 2.0 - 1.0, (1.0 - texCoord.y) * 2.0 - 1.0, zOverW, 1.0);
		vec4 D = inverseViewProjection * H;
		vec4 worldPos = D / D.w;

		// Compute velocity vector
		vec4 currentPos = H;
		vec4 previousPos = previousViewProjection * worldPos;
		previousPos /= previousPos.w;
		vec2 velocity = ((currentPos - previousPos) / 2.0).xy;

		// Sample the color sampler using the velocity vector to achieve motion blur
		texCoord -= velocity;
		for (int i = 0; i < NUM_VELOCITY_SAMPLES; ++i, texCoord -= (velocity * velocity * NUM_VELOCITY_SAMPLES * 0.5)) {
			vec4 currentColor = texture(colorSampler, texCoord) * 0.5;
			color += currentColor;
		}
		color /= NUM_VELOCITY_SAMPLES;
	}

	FragColor = color;
}
