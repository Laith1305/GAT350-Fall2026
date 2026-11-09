#pragma once
#include "Component.h"
namespace nu {
	class CameraControllerComponent : public Component
	{



	public:
		CLASS_PROTOTYPE(CameraControllerComponent)
		void Update(float dt) override;
		
		void Read(const json::value_t& value) override;

	private:
		float m_speed{ 4.0f };
		float m_sensitivity{ 0.1f };
		float m_yaw{ 0.0f };
		float m_pitch{ 0.0f };
	};
}