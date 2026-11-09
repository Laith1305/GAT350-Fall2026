#pragma once
#include "Resources/Resource.h"
#include "Material.h"

namespace nu
{
	class Texture;

	class MaterialInstance : public Resource
	{
	public:
		MaterialInstance() = default;
		explicit MaterialInstance(const res_t<Material> material);

		void Initialize(const res_t<Material>& material);
		bool Load(const std::string& filename, const class Renderer& renderer);

		void Bind(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass) const;

		void SetTexture(size_t slot, res_t<Texture> texture);
		bool SetTexture(const std::string& name, res_t<Texture> texture);

		MaterialData& GetData() { return m_data; }
		const MaterialData& GetData() const { return m_data; }

		const Material& GetMaterial() const { return *m_material; }

	private:
		res_t<Material> m_material;
		MaterialData m_data;
		std::vector<res_t<class Texture>> m_textures;
	};
}