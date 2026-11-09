#include "pch.h"
#include "ModelComponent.h"
#include "Core/Factory.h"
#include "Renderer/Uniforms.h"
#include "Framework/Actor.h"
#include <Resources/ResourceManager.h>
#include "Engine.h"


namespace nu
{
	FACTORY_REGISTER(ModelComponent)

	void ModelComponent::Draw(const Renderer& renderer)
	{
		ObjectUniform objectUniform{
			.model = m_owner->GetTransform().GetMatrix(),
			.normalMatrix = m_owner->GetTransform().GetMatrix(),

		};

		renderer.SetVertexUniform(UniformSlot::Frame, &objectUniform, sizeof(ObjectUniform));
		renderer.SetFragmentUniform(UniformSlot::Frame, &objectUniform, sizeof(ObjectUniform));

		m_model->Draw(renderer);
	}
	void ModelComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		std::string modelName;
		JSON_READ_NAME(value, "model", modelName);

		m_model = Resources().Get<Model>(modelName, Engine::Instance().GetRenderer());
	}
}