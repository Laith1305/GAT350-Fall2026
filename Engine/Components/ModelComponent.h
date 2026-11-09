#pragma once
#include "RendererComponent.h"
#include "Resources/Resource.h"

namespace nu
{
	class ModelComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(ModelComponent)

		void Draw(const Renderer& renderer) override;
		void Read(const json::value_t& value) override;

	private:
		res_t<class Model> m_model;
	};
}