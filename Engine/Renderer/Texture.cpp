#include "pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Core/File.h"
#include <SDL3_image/SDL_image.h>

namespace nu
{
	Texture::~Texture()
	{
		// if the gpu device is valid (not null), release the sampler and texture
		// with SDL_ReleaseGPUSampler(device, sampler) and SDL_ReleaseGPUTexture(device, texture)
		if (m_gpuDevice)
		{
			if (m_gpuSampler) SDL_ReleaseGPUSampler(m_gpuDevice, m_gpuSampler);
			if (m_gpuTexture) SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
		}
	}

	bool Texture::Load(const std::string& filename, const Renderer& renderer)
	{
		if (GetFileExtension(filename) == ".json")
		{
			return LoadCubeMap(filename, renderer);
		}

		return Load2D(filename, renderer);
	}

	bool Texture::Load2D(const std::string& filename, const Renderer& renderer)
	{
		m_gpuDevice = renderer.GetGPUDevice();
		m_type = TextureType::Texture2D;

		// load image to surface
		SDL_Surface* surface = LoadSurface(filename);
		if (surface == nullptr) return false;

		// create texture
		SDL_GPUTextureCreateInfo createInfo{
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = static_cast<uint32_t>(surface->w),
			.height = static_cast<uint32_t>(surface->h),
			.layer_count_or_depth = 1,
			.num_levels = 1
		};

		bool success = CreateGPUTexture(createInfo);
		if (success)
		{
			// upload texture to the gpu
			success = UploadTextureData({ surface });
		}

		// destroy image surface (already transferred to GPU)
		SDL_DestroySurface(surface);
		if (!success) return false;

		// create texture sampler
		return CreateSampler();
	}

	bool Texture::LoadCubeMap(const std::string& filename, const Renderer& renderer)
	{
		m_gpuDevice = renderer.GetGPUDevice();
		m_type = TextureType::Cube;

		// get cube faces filenames
		json::document_t document;
		json::Load(filename, document);
		std::vector<std::string> filenames;
		JSON_READ_NAME(document, "faces", filenames);

		// load all six cube-map faces
		std::vector<SDL_Surface*> surfaces;
		for (const std::string& filename : filenames)
		{
			SDL_Surface* surface = LoadSurface(filename);
			// destroy all surfaces if one fails
			if (surface == nullptr)
			{
				for (SDL_Surface* s : surfaces)
				{
					SDL_DestroySurface(s);
				}

				return false;
			}

			surfaces.push_back(surface);
		}

		// get dimensions from the first face
		m_width = static_cast<uint32_t>(surfaces[0]->w);
		m_height = static_cast<uint32_t>(surfaces[0]->h);

		// cube-map faces must be square and the same size
		for (SDL_Surface* surface : surfaces)
		{
			if (surface->w != static_cast<int>(m_width) ||
				surface->h != static_cast<int>(m_height) ||
				m_width != m_height)
			{
				std::cerr << "Invalid cube-map face dimensions." << std::endl;
				// destroy all surfaces if one fails
				for (SDL_Surface* s : surfaces)
				{
					SDL_DestroySurface(s);
				}

				return false;
			}
		}

		// create cube-map texture
		SDL_GPUTextureCreateInfo createInfo{
			.type = SDL_GPU_TEXTURETYPE_CUBE,
			.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
			.width = m_width,
			.height = m_height,
			.layer_count_or_depth = 6,
			.num_levels = 1
		};

		bool success = CreateGPUTexture(createInfo);
		if (success)
		{
			// update texture to the gpu
			success = UploadTextureData(surfaces);
		}

		// destroy image surface (already transferred to GPU)
		for (SDL_Surface* surface : surfaces)
		{
			SDL_DestroySurface(surface);
		}

		if (!success)
		{
			return false;
		}

		// create sampler
		return CreateSampler();
	}

	SDL_Surface* Texture::LoadSurface(const std::string& filename)
	{
		// load the image
		SDL_Surface* loadedSurface = IMG_Load(filename.c_str());
		if (loadedSurface == nullptr)
		{
			std::cerr << "Could not load image " << filename << ": " << SDL_GetError() << std::endl;
			return nullptr;
		}

		// convert loaded surface to surface format
		SDL_Surface* surface = SDL_ConvertSurface(
			loadedSurface,
			SDL_PIXELFORMAT_ABGR8888
		);

		// destroy loaded surface
		SDL_DestroySurface(loadedSurface);

		if (surface == nullptr)
		{
			std::cerr << "Could not convert image " << filename << ": " << SDL_GetError() << std::endl;
			return nullptr;
		}

		return surface;
	}

	bool Texture::CreateGPUTexture(const SDL_GPUTextureCreateInfo& createInfo)
	{
		// create gpu texture from create info
		m_gpuTexture = SDL_CreateGPUTexture(m_gpuDevice, &createInfo);
		if (m_gpuTexture == nullptr)
		{
			std::cerr << "Could not create GPU texture: " << SDL_GetError() << std::endl;
			return false;
		}

		// set width and height
		m_width = createInfo.width;
		m_height = createInfo.height;

		return true;
	}

	bool Texture::UploadTextureData(const std::vector<SDL_Surface*>& surfaces)
	{
		// check for surfaces
		if (surfaces.empty() || m_gpuTexture == nullptr)
		{
			return false;
		}

		// Calculate the total size of all images
		uint32_t totalSize = 0;

		for (SDL_Surface* surface : surfaces)
		{
			// check for valis surface
			if (surface == nullptr ||
				surface->w != static_cast<int>(m_width) ||
				surface->h != static_cast<int>(m_height) ||
				surface->format != SDL_PIXELFORMAT_ABGR8888 ||
				surface->pitch < static_cast<int>(m_width * 4))
			{
				std::cerr << "Invalid texture surface." << std::endl;
				return false;
			}

			// add total size of all surfaces
			totalSize += static_cast<uint32_t>(surface->pitch) * surface->h;
		}

		// create transfer buffer, upload surface to gpu
		SDL_GPUTransferBufferCreateInfo transferCreateInfo{
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = totalSize
		};

		SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_gpuDevice, &transferCreateInfo);
		if (transferBuffer == nullptr)
		{
			std::cerr << "Could not create transfer buffer: " << SDL_GetError() << std::endl;
			return false;
		}

		// map transfer buffer
		uint8_t* transferData = static_cast<uint8_t*>(SDL_MapGPUTransferBuffer(m_gpuDevice, transferBuffer, false));
		if (transferData == nullptr)
		{
			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			return false;
		}

		// copy all surfaces into the transfer buffer
		uint32_t offset = 0;
		for (SDL_Surface* surface : surfaces)
		{
			uint32_t imageSize = static_cast<uint32_t>(surface->pitch) * surface->h;
			memcpy(transferData + offset, surface->pixels, imageSize);

			offset += imageSize;
		}

		SDL_UnmapGPUTransferBuffer(m_gpuDevice, transferBuffer);

		// acquire command buffer
		SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpuDevice);
		if (commandBuffer == nullptr)
		{
			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			return false;
		}

		// begin copy pass
		SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
		if (copyPass == nullptr)
		{
			SDL_CancelGPUCommandBuffer(commandBuffer);
			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);
			return false;
		}

		// upload each image to a texture layer
		offset = 0;
		for (uint32_t i = 0; i < surfaces.size(); i++)
		{
			SDL_Surface* surface = surfaces[i];

			uint32_t imageSize = static_cast<uint32_t>(surface->pitch) * surface->h;
			SDL_GPUTextureTransferInfo transferInfo{
				.transfer_buffer = transferBuffer,
				.offset = offset,
				.pixels_per_row = static_cast<uint32_t>(surface->pitch) / 4,
				.rows_per_layer = m_height
			};

			SDL_GPUTextureRegion textureRegion{
				.texture = m_gpuTexture,
				.mip_level = 0,
				.layer = i,
				.x = 0,
				.y = 0,
				.z = 0,
				.w = m_width,
				.h = m_height,
				.d = 1
			};

			SDL_UploadToGPUTexture(
				copyPass,
				&transferInfo,
				&textureRegion,
				false
			);

			offset += imageSize;
		}

		SDL_EndGPUCopyPass(copyPass);

		// submit GPU commands
		if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
		{
			std::cerr << "Could not submit texture upload: " << SDL_GetError() << std::endl;
			SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);

			return false;
		}

		SDL_ReleaseGPUTransferBuffer(m_gpuDevice, transferBuffer);

		return true;
	}

	bool Texture::CreateSampler()
	{
		// fill out the sampler settings
		SDL_GPUSamplerCreateInfo samplerCreateInfo{
			.min_filter = SDL_GPU_FILTER_LINEAR, // todo: smooth when the texture is shrunk (SDL_GPU_FILTER_...)
			.mag_filter = SDL_GPU_FILTER_LINEAR, // todo: smooth when the texture is enlarged (SDL_GPU_FILTER_...)
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT, // todo: tile the texture horizontally (SDL_GPU_SAMPLERADDRESSMODE_...)
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT, // todo: tile the texture vertically (SDL_GPU_SAMPLERADDRESSMODE_...)
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT
		};

		// create the sampler with SDL_CreateGPUSampler(device, &createInfo) and store it in m_gpuSampler
		m_gpuSampler = SDL_CreateGPUSampler(m_gpuDevice, &samplerCreateInfo);
		if (m_gpuSampler == nullptr)
		{
			std::cerr << "Could not create sampler: " << SDL_GetError() << std::endl;

			SDL_ReleaseGPUTexture(m_gpuDevice, m_gpuTexture);
			m_gpuTexture = nullptr;

			return false;
		}

		return true;
	}

	void Texture::UpdateGui()
	{
		Resource::UpdateGui();
		float width = ImGui::GetContentRegionAvail().x * 0.5f;
		ImGui::Image(m_gpuTexture, ImVec2{ width, width * (m_height / (float)m_width) });
		ImGui::Text("%d x %d", m_width, m_height);
	}


}
