#define GLM_ENABLE_EXPERIMENTAL 
#include "Engine.h"
#include "Core/File.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Resources/ResourceManager.h>
#include <Renderer/VertexBuffer.h>
#include <Renderer/Pipeline.h>
#include <Renderer/Shader.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>


using namespace nu;




struct Color4 {
    float r, g, b, a;
};
struct Vertex
{
	Vector3 position;
    Color4 color;
};


struct vertexUniform
{
    glm::mat4 mvp;
};

vertexUniform uniform;
glm::vec3 cameraPosition{ 0, 0, 5 };

std::vector<Vertex> vertices =
{
    Vertex{ Vector3{ -1.0f, -1.0f, 0.0f }, Color4{1, 0, 0, 1}}, // Bottom-Left
    Vertex{ Vector3{  1.0f, -1.0f, 0.0f }, Color4{0, 1, 0, 1}}, // Bottom-Right
    Vertex{ Vector3{  1.0f,  1.0f, 0.0f }, Color4{0, 0, 1, 1}}, // Top-Right
    Vertex{ Vector3{  -1.0f,  1.0f, 0.0f }, Color4{0, 1, 1, 1}}, // Top-Left


};


int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();
    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Instance().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
	//position
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, position));
    //color
    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        offsetof(Vertex, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());

    float scale = 0.5f;
	Color4 color = { 1, 0, 1, 1 };

    // MAIN LOOP
    bool quit = false;
    while (!quit) 
    {
        // UPDATE
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {



            

            if (event.type == SDL_EVENT_QUIT) 
            {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                quit = true;
            }
        }



        // ENGINE
        Engine::Instance().Update();
        float dt = Engine::Instance().GetTime().GetDeltaTime();
        float time = Engine::Instance().GetTime().GetTime();
        scale = sin(time);
        color.r = sin(time);

        glm::vec3 direction{ 0 };
        if(Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_A)) { direction.x -= 1;}
        if(Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_D)) { direction.x += 1;}
        if(Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_W)) { direction.z += 1;}
        if(Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_S)) { direction.z -= 1;}

		cameraPosition += direction * 5.0f * dt;



		//uniform.model = glm::mat4{ 1.0f };
        //uniform.model = glm::translate(uniform.model, glm::vec3(-0.5f, -0.3f, 1.0f));
        //uniform.model = glm::rotate(uniform.model, time * 3.0f, glm::vec3(0.0f, 1.0f, 1.0f));

        auto mxt = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.0f, 0.3f, 1.0f));
		auto mxr = glm::eulerAngleYXZ(0.0f, 0.0f, 0.0f);

        auto mxs = glm::scale(glm::vec3{ 1.0f });

		auto model = mxt * mxr * mxs;
		uniform.mvp = model;

        /*auto mxrx = glm::rotate(glm::mat4{ 1.0f },time * 3.0f, glm::vec3(1.0f, -0.3f, 1.0f));
        auto mxry = glm::scale(glm::mat4{ 1.0f }, glm::vec3(3.0f, -0.5f, -0.3f));*/

        //VIEW MATRIX
		auto view = glm::lookAtRH(cameraPosition, cameraPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


		//PROJECTION MATRIX
		float aspectRatio = Engine::Instance().GetRenderer().GetWidth() / static_cast<float>(Engine::Instance().GetRenderer().GetHeight());
		auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 20.0f);
		
		uniform.mvp = projection * view * model;
        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);

        Engine::Instance().GetRenderer().SetVertexBuffer(*vb);
		Engine::Instance().GetRenderer().SetVertexUniform(0, &scale, sizeof(scale));
		Engine::Instance().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));

        Engine::Instance().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Instance().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Instance().GetRenderer().EndFrame();

    }

   

    // SHUTDOWN
    Engine::Instance().Shutdown();    

    return 0;
}

