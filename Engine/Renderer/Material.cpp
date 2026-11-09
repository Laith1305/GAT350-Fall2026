#include "pch.h"
#include "Material.h"
#include "Core/File.h"
#include "Serialization/Json.h"
#include "Resources/ResourceManager.h"
#include "Renderer.h"
#include "Model.h"
#include "Texture.h"

namespace nu
{
	bool Material::Load(const std::string& filename, const class Renderer& renderer)
	{
		json::document_t document;
		if (!json::Load(filename, document))
		{
			std::cerr << "Could not load Material: " << filename << std::endl;
			return false;
		}

		// make sure this is a material template, not an instance
		std::string type;
		JSON_READ(document, type);
		if (type != "material")
		{
			std::cerr << "Not a material file (type: \"" << type << "\"): " << filename << std::endl;
			return false;
		}

		JSON_READ_NAME(document, "base_color", m_defaults.baseColor);
		JSON_READ_NAME(document, "tiling", m_defaults.tiling);
		JSON_READ_NAME(document, "offset", m_defaults.offset);
		JSON_READ_NAME(document, "shininess", m_defaults.shininess);

		// texture slots in sampler order, slot 0 = register(t0, space2)
		JSON_READ_NAME(document, "textures", m_textureSlots);

		m_defaultTexture = Resources().Get<Texture>("textures/default.png", renderer);
		if (!m_defaultTexture)
		{
			std::cerr << "Could not load default texture for Material: " << filename << std::endl;
			return false;
		}

		// shaders
		std::string vertexShaderName;
		JSON_READ_NAME(document, "vertex_shader", vertexShaderName);
		if (!vertexShaderName.empty()) 
			m_vertexShader = Resources().Get<nu::Shader>(vertexShaderName, renderer);

		std::string fragmentShaderName;
		JSON_READ_NAME(document, "fragment_shader", fragmentShaderName);
		if (!fragmentShaderName.empty())
			m_fragmentShader = Resources().Get<nu::Shader>(fragmentShaderName, renderer);

		if (!m_vertexShader || !m_fragmentShader)
		{
			std::cerr << "Could not load Material shaders: " << filename << std::endl;
			return false;
		}

		m_pipeline.AddVertexBuffer(sizeof(Model::Vertex));
		m_pipeline.AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
		m_pipeline.AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
		m_pipeline.AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
		m_pipeline.AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

		if (!m_pipeline.Create(*m_vertexShader, *m_fragmentShader, renderer.GetGPUDevice(), renderer.GetWindow()))
		{
			std::cerr << "Could not create pipeline for Material: " << filename << std::endl;
			return false;
		}

		return true;
	}

	int Material::GetTextureSlot(const std::string& name) const
	{
		auto it = std::find(m_textureSlots.begin(), m_textureSlots.end(), name);
		return (it != m_textureSlots.end()) ? (int)std::distance(m_textureSlots.begin(), it) : -1;
	}
}