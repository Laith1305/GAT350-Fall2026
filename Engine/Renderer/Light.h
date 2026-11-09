#pragma once
#include <glm/glm.hpp>
#include <cstdint>

namespace nu
{
	constexpr uint32_t MAX_LIGHTS = 8;   // must match MAX_LIGHTS in the shader

	// values must match the light type constants in the shader
	enum class LightType : uint32_t
	{
		Directional = 0,
		Point = 1,
		Spot = 2
	};

	// gpu layout of one light, must match the Light struct in the shader
	struct LightData
	{
		glm::vec3 position;     // world space (point, spot)
		LightType type;
		glm::vec3 direction;    // world space, normalized, direction the light travels (directional, spot)
		float range;            // distance where the light reaches zero (point, spot)
		glm::vec3 color;
		float intensity;
		float innerCutoff;      // cos of the inner cone angle (spot)
		float outerCutoff;      // cos of the outer cone angle (spot)
		glm::vec2 padding;
	};

	struct LightUniform
	{
		LightData lights[MAX_LIGHTS];
		glm::vec3 ambient;      // scene-wide ambient, not per light
		uint32_t count;
	};
}