#pragma once
#include "Uniforms.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace nu
{
	class Renderer
	{
	public:
		bool Initialize(const char* name, int width, int height);
		void Shutdown();

		void Clear() const;
		void Present() const;


		bool BeginFrame();
		bool EndFrame();

		void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) const;
		void SetColor(float r, float g, float b, float a = 1.0f) const;
		
	

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		// SDL GPU

		SDL_GPUDevice* GetGPUDevice() const { return m_gpuDevice; }
		SDL_Window* GetWindow() const { return m_window; }

		void SetPipeline(const class Pipeline& pipeline);
		void SetVertexBuffer(const class VertexBuffer& vertexBuffer) const;
		void SetIndexBuffer(const class IndexBuffer& indexBuffer) const;
		void Draw(uint32_t vertexCount) const;
		void DrawIndex(const class IndexBuffer& indexBuffer) const;

		void SetVertexUniform(UniformSlot slot, const void* data, uint32_t size) const;
		void SetFragmentUniform(UniformSlot slot, const void* data, uint32_t size) const;

		void SetFragmentTexture(uint32_t slot, const class Texture& texture) const;

		void SetLightUniform(uint32_t slot, const void* data, uint32_t size);


		void CreateDepthBuffer();


		friend class TextRenderer;
		friend class Texture;
		friend class Shader;
		friend class VertexBuffer;
		friend class IndexBuffer;

	private:
		SDL_Renderer* m_renderer = nullptr;
		SDL_Window* m_window = nullptr;

		SDL_GPUDevice* m_gpuDevice = nullptr;
		SDL_GPUCommandBuffer* m_commandBuffer = nullptr;
		SDL_GPURenderPass* m_renderPass = nullptr;
		SDL_GPUTexture* m_depthTexture = nullptr;


		int m_width = 0;
		int m_height = 0;
	};

	

}

