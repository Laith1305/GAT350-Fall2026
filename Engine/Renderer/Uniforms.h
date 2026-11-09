#pragma once
#include <glm/glm.hpp>
#include <cstdint>

#define CHECK_UNIFORM_BOUNDARY(T) static_assert((sizeof(T) % 16) == 0)

namespace nu
{
	// uniform slots, must match shader slots
	enum class UniformSlot : uint32_t
	{
		Object = 0,
		Frame = 1,
		Light = 2,
		Material = 3
	};

	// pushed once per frame
	struct FrameUniform
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;   // projection * view, computed once on the cpu
		glm::vec3 cameraPosition;
		float time;
	};
	CHECK_UNIFORM_BOUNDARY(FrameUniform);

	// pushed once per draw
	struct ObjectUniform
	{
		glm::mat4 model;
		glm::mat4 normalMatrix;     // transpose(inverse(model))
	};
	CHECK_UNIFORM_BOUNDARY(ObjectUniform);
}
