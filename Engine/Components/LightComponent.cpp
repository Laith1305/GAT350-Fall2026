#include "pch.h"
#include "LightComponent.h"
#include "Core/Factory.h"
#include <Math/Transform.h>
#include "Framework/Actor.h"

namespace nu
{
	FACTORY_REGISTER(LightComponent)

	void LightComponent::Draw(const Renderer& renderer)
	{

	}

	void LightComponent::Read(const json::value_t& value)
	{
		std::string typeName;
		JSON_READ_NAME(value, "light_type", typeName);
		if (typeName == "directional") m_type = LightType::Directional;
		else if (typeName == "spot") m_type = LightType::Spot;
		else m_type = LightType::Point;

		JSON_READ_NAME(value, "color", m_color);
		JSON_READ_NAME(value, "intensity", m_intensity);
		JSON_READ_NAME(value, "range", m_range);
		JSON_READ_NAME(value, "inner_angle", m_innerAngle);
		JSON_READ_NAME(value, "outer_angle", m_outerAngle);
	}

	LightData LightComponent::GetLightData() const
	{
		Transform transform = m_owner->GetTransform();

		LightData lightData = {
		.position = transform.position,
		.type = m_type,
		.direction = transform.Forward(),
		.range = m_range,
		.color = m_color,
		.intensity = m_intensity,
		.innerCutoff = glm::cos(glm::radians(m_innerAngle)),
		.outerCutoff = glm::cos(glm::radians(m_outerAngle))
		};
		return lightData;
	}
}