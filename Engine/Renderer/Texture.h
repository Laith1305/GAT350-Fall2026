#pragma once
#include "Resources/Resource.h"

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include <string>
#include <array>
#include <vector>

namespace nu
{
	class Texture : public Resource
	{
	public:
		enum class TextureType
		{
			Texture2D,
			Cube
		};

	public:
		~Texture();

		bool Load(const std::string& filename, const class Renderer& renderer);

		bool Load2D(const std::string& filename, const class Renderer& renderer);
		bool LoadCubeMap(const std::string& filename, const Renderer& renderer);

		TextureType GetType() const { return m_type; }

		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		glm::vec2 GetSize() const { return { m_width, m_height }; }

		SDL_GPUTexture* GetGPUTexture() const { return m_gpuTexture; }
		SDL_GPUSampler* GetGPUSampler() const { return m_gpuSampler; }
		

		void UpdateGui() override;

	private:
		SDL_Surface* LoadSurface(const std::string& filename);
		bool CreateGPUTexture(const SDL_GPUTextureCreateInfo& createInfo);
		bool UploadTextureData(const std::vector<SDL_Surface*>& surfaces);
		bool CreateSampler();

	private:
		TextureType m_type{ TextureType::Texture2D };

		uint32_t m_width{ 0 };
		uint32_t m_height{ 0 };

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUTexture* m_gpuTexture = nullptr;
		SDL_GPUSampler* m_gpuSampler = nullptr;
	};
}
