#pragma once
#include "RendererComponent.h"


namespace nu
{
	class CameraComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(CameraComponent)

		void Start() override;
		void Update(float dt) override;
		void Draw(const Renderer& renderer) override;

		void Read(const json::value_t& value) override;

		const glm::mat4x4& GetView() const { return m_view; }
		const glm::mat4x4& GetProjection() const { return m_projection; }

	private:
		glm::mat4x4 m_view;
		glm::mat4x4 m_projection;

		float m_fov{ 70.0f };
		float m_nearPlane{ 0.01f };
		float m_farPlane{ 300.0f };
		float m_aspectRatio{ 16.0f / 9.0f };
	};
}