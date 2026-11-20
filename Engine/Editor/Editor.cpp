#include "pch.h"
#include "Editor.h"
#include "Renderer/Renderer.h"
#include "Framework/Scene.h"
#include "Framework/Object.h"
#include "Resources/ResourceManager.h"
namespace nu
{
	bool Editor::Initialize(Renderer& renderer)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();

		// platform backend: input, window size, cursor
		if (!ImGui_ImplSDL3_InitForSDLGPU(renderer.GetWindow()))
		{
			std::cerr << "Could not initialize ImGui SDL3 backend" << std::endl;
			return false;
		}

		// renderer backend: its pipeline must target the swapchain format
		ImGui_ImplSDLGPU3_InitInfo initInfo{};
		initInfo.Device = renderer.GetGPUDevice();
		initInfo.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(renderer.GetGPUDevice(), renderer.GetWindow());
		initInfo.MSAASamples = SDL_GPU_SAMPLECOUNT_1;

		if (!ImGui_ImplSDLGPU3_Init(&initInfo))
		{
			std::cerr << "Could not initialize ImGui SDL GPU backend" << std::endl;
			return false;
		}

		return true;
	}

	void Editor::Shutdown()
	{
		ImGui_ImplSDLGPU3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::ProcessEvent(const SDL_Event& event)
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
	}

	void Editor::Update(class Scene& scene)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) 
			m_active = !m_active;
		if (!m_active) return;
		
		ImGui::ShowDemoWindow();
	}

	void Editor::BeginFrame()
	{
		ImGui_ImplSDLGPU3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::Draw(Renderer& renderer)
	{
		// always finish the imgui frame, even if nothing is drawn, or the next NewFrame asserts
		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();

		// skip drawing when minimized (no swapchain texture) or there's nothing to draw
		SDL_GPUTexture* swapchainTexture = nullptr;
		if (!swapchainTexture || drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f) return;

		SDL_GPUCommandBuffer* commandBuffer = renderer.GetGPUCommandBuffer();

		// upload vertex and index data, must happen outside a render pass
		ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuffer);

		// second pass on top of the scene: load keeps the scene's pixels, no depth target
		SDL_GPUColorTargetInfo target{};
		target.texture = swapchainTexture;
		target.load_op = SDL_GPU_LOADOP_LOAD;
		target.store_op = SDL_GPU_STOREOP_STORE;

		SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &target, 1, nullptr);
		ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuffer, renderPass);
		SDL_EndGPURenderPass(renderPass);
	}
	void Editor::UpdateHierarchy(Scene& scene)
	{
		ImGui::Begin("Heirarchy");
		scene.UpdateGui();
		ImGui::Separator();

		for(auto& actor : scene.GetActors())
		{
			ImGui::PushID(actor.get());
			if (ImGui::Selectable(actor->GetName().c_str(), m_selected == actor.get()))
			{
				m_selected = actor.get();
			}
			ImGui::PopID();
		}

		ImGui::End();
	}

	void Editor::UpdateInspector()
	{
		ImGui::Begin("Inspector");
		if(m_selected)
		{
			m_selected->UpdateGui();

		}
		ImGui::End();
	}

	void Editor::UpdateAssets()
	{
		ImGui::Begin("Assets");

		for (auto resource : Resources().GetAll()) {
			ImGui::PushID(resource.second.get());
			if (ImGui::Selectable(resource.second->GetName().c_str(), m_selected == resource.second.get()))
			{
				m_selected = resource.second.get();
			}
			ImGui::Text("%s", resource.second->GetName().c_str());
			ImGui::PopID();
		}
		ImGui::End();
	}


}