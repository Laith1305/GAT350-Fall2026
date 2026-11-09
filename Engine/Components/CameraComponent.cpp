#include "pch.h"
#include "CameraComponent.h"
#include "Core/Factory.h"
#include "Framework/Actor.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(CameraComponent)

	void CameraComponent::Start()
	{
		m_aspectRatio = Engine::Instance().GetRenderer().GetWidth() / static_cast<float>(Engine::Instance().GetRenderer().GetHeight());
	}

	void CameraComponent::Update(float dt)
	{
		glm::vec3 forward = m_owner->GetTransform().Forward();
		glm::vec3 up = m_owner->GetTransform().Up();
		glm::vec3 right = m_owner->GetTransform().Right();
		m_view = glm::lookAtRH(m_owner->GetTransform().position, m_owner->GetTransform().position + forward, up);
		m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
	}

	void CameraComponent::Draw(const Renderer& renderer)
	{

	}

	void CameraComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		JSON_READ_NAME(value, "fov", m_fov);
		JSON_READ_NAME(value, "near_plane", m_nearPlane);
		JSON_READ_NAME(value, "far_plane", m_farPlane);
	}

}