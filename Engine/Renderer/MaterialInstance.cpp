#include "pch.h"
#include "MaterialInstance.h"
#include "Texture.h"
#include "Uniforms.h"
#include "Serialization/Json.h"
#include "Resources/ResourceManager.h"

namespace nu
{
	MaterialInstance::MaterialInstance(const res_t<Material> material)
	{
		Initialize(material);
	}

	void MaterialInstance::Initialize(const res_t<Material>& material)
	{
		m_material = material;
		m_data = m_material->GetDefaults();
		m_textures.assign(m_material->GetTextureSlotCount(), nullptr);
	}

	bool MaterialInstance::Load(const std::string& filename, const Renderer& renderer)
	{
		json::document_t document;
		if (!json::Load(filename, document))
		{
			std::cerr << "Could not load MaterialInstance: " << filename << std::endl;
			return false;
		}

		// make sure this is an instance file, not a material template
		std::string type;
		JSON_READ(document, type);
		if (type != "material_instance")
		{
			std::cerr << "Not a material instance file (type: \"" << type << "\"): " << filename << std::endl;
			return false;
		}

		// the material comes first, all values start from its defaults
		std::string materialName;
		JSON_READ_NAME(document, "material", materialName);
		auto material = Resources().Get<Material>(materialName, renderer);
		if (!material)
		{
			std::cerr << "Could not load Material \"" << materialName << "\" for instance: " << filename << std::endl;
			return false;
		}
		Initialize(material);

		// only values present in the file override the defaults
		JSON_READ_NAME(document, "base_color", m_data.baseColor);
		JSON_READ_NAME(document, "tiling", m_data.tiling);
		JSON_READ_NAME(document, "offset", m_data.offset);
		JSON_READ_NAME(document, "shininess", m_data.shininess);

		// textures by slot name: "textures": { "base_map": "textures/brick.png" }
		if (JSON_HAS_NAME(document, "textures"))
		{
			for (const auto& entry : JSON_GET_NAME(document, "textures").GetObject())
			{
				std::string slot = entry.name.GetString();
				std::string path = entry.value.GetString();

				auto texture = Resources().Get<Texture>(path, renderer);
				if (!texture)
				{
					std::cerr << "Could not load texture \"" << path << "\" for instance: " << filename << std::endl;
					continue;
				}

				if (!SetTexture(slot, texture))
				{
					std::cerr << "Material has no texture slot \"" << slot << "\" in instance: " << filename << std::endl;
				}
			}
		}

		return true;
	}

	void MaterialInstance::Bind(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass) const
	{
		// empty slots fall back to a default texture so the shader can always sample
		std::vector<SDL_GPUTextureSamplerBinding> bindings;
		bindings.reserve(m_textures.size());
		for (const auto& texture : m_textures)
		{
			const Texture& source = texture ? *texture : *m_material->GetDefaultTexture();
			bindings.push_back({ source.GetGPUTexture(), source.GetGPUSampler() });
		}

		if (!bindings.empty())
		{
			SDL_BindGPUFragmentSamplers(renderPass, 0, bindings.data(), (uint32_t)bindings.size());
		}

		// same struct to both stages, each uses the fields it needs
		SDL_PushGPUVertexUniformData(commandBuffer, (uint32_t)UniformSlot::Material, &m_data, sizeof(m_data));
		SDL_PushGPUFragmentUniformData(commandBuffer, (uint32_t)UniformSlot::Material, &m_data, sizeof(m_data));
	}

	void MaterialInstance::SetTexture(size_t slot, res_t<Texture> texture)
	{
		if (slot < m_textures.size()) m_textures[slot] = std::move(texture);
	}

	bool MaterialInstance::SetTexture(const std::string& name, res_t<Texture> texture)
	{
		int slot = m_material->GetTextureSlot(name);
		if (slot < 0) return false;

		m_textures[slot] = texture;
		return true;
	}
}