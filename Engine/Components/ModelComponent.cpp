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
		if (!m_model) {
			std::cerr << "could not draw model for actor " << m_owner->GetName() << std::endl;
			return;
		}


		ObjectUniform objectUniform{
			.model = m_owner->GetTransform().GetMatrix(),
			.normalMatrix = m_owner->GetTransform().GetMatrix(),

		};

		renderer.SetVertexUniform(UniformSlot::Frame, &objectUniform, sizeof(ObjectUniform));
		renderer.SetFragmentUniform(UniformSlot::Frame, &objectUniform, sizeof(ObjectUniform));

		m_model->Draw(renderer, m_materials);
	}
	void ModelComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		std::string modelName;
		JSON_READ_NAME(value, "model", modelName);

		m_model = Resources().Get<Model>(modelName, Engine::Instance().GetRenderer());

		// "materials": [ "materials/brick.instance.json", null ] - one entry per slot, null keeps the model's material
		if (JSON_HAS_NAME(value, "materials"))
		{
			for (const auto& entry : JSON_GET_NAME(value, "materials").GetArray())
			{
				if (entry.IsString())
				{
					auto material = Resources().Get<MaterialInstance>(entry.GetString(), Engine::Instance().GetRenderer());
					if (!material) std::cerr << "Could not load material: " << entry.GetString() << std::endl;
					m_materials.push_back(material);
				}
				else
				{
					m_materials.push_back(nullptr);
				}
			}
		}


	}
}