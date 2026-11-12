#include "Engine.h"
#include "Core/File.h"
#include "Math/Transform.h"
#include "Renderer/Model.h"
#include "Framework/Scene.h"
#include <Resources/ResourceManager.h>
#include <Renderer/VertexBuffer.h>
#include <Renderer/IndexBuffer.h>
#include <Renderer/Pipeline.h>
#include <Renderer/Shader.h>

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Components/CameraComponent.h"


using namespace nu;


struct Vertex
{
    glm::vec3 position;
    glm::vec4 color;
};




int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    Scene scene;
	scene.Load("Scenes/scene.json");


    struct Transform
    {
        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 0.0f };
        glm::vec3 scale{ 1.0f };
        glm::mat4 matrix;
        const glm::mat4& GetMatrix()
        {
            auto mxt = glm::translate(position);
            auto mxr = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
            auto mxs = glm::scale(scale);
            matrix = mxt * mxr * mxs;
            return matrix;
        }
    };

    float scale = 0.5f;
    glm::vec4 color = { 1, 1, 1, 1 };

    auto& renderer = Engine::Instance().GetRenderer();

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

       
		scene.Update(dt);

       
        //MVP 
		
        renderer.BeginFrame();

		scene.Draw(renderer);

        renderer.EndFrame();




    }



    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

