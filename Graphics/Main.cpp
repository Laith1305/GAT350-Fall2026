#include "Engine.h"
#include "Core/File.h"
#include "Math/Transform.h"
#include "Renderer/Model.h"
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

#define CHECK_UNIFORM_BOUNDARY(T) static_assert((sizeof(T) % 16) == 0);


using namespace nu;

enum LightType : uint32_t
{
    Directional = 0,
    Point = 1,
    Spot = 2
};

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


    auto model = Resources().Get<Model>("models/spot.obj", Engine::Instance().GetRenderer());
    //auto texture = Resources().Get<Texture>("models/spot_diffuse.png", Engine::Instance().GetRenderer());


    auto vshader = Resources().Get<nu::Shader>("shaders/phong.vert", Engine::Instance().GetRenderer(), 3);
    auto fshader = Resources().Get<nu::Shader>("shaders/phong.frag", Engine::Instance().GetRenderer(), 3, 1);

    auto pipeline = std::make_shared<Pipeline>();


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

    //Uniforms
    glm::vec4 Color4 { 1, 1, 1, 1 };
    struct VertexUniform
    {
        glm::mat4 model;
        glm::mat4 mvp;
		glm::vec3 cameraPosition;
        float padding1;
    };

    CHECK_UNIFORM_BOUNDARY(VertexUniform);

    VertexUniform vertexUniform;
    vertexUniform.cameraPosition = glm::vec3{ 0, 0, 5 };



    struct LightUniform
    {
        glm::vec3 position;
	    uint32_t type;
        glm::vec3 direction;
        float range;
        glm::vec3 color;
        float intensity;
        glm::vec3 ambient;
        float innerCutoff;
        float outerCutoff;
        glm::vec3 padding;
    };

    LightUniform lightUniform{
	    .position{10, 10, 10},
        .type{LightType::Point},
  	    .direction{0, -1, 0},  
        .range{ 40 },
	    .color{1, 1, 1},
	    .intensity{1.0f},
        .ambient{0.1f, 0.1f, 0.1f},
        .innerCutoff{0.3f},
        .outerCutoff{1.6f}
    };

    CHECK_UNIFORM_BOUNDARY(LightUniform);




    struct MaterialUniform {
        glm::vec4 color;
        glm::vec2 tiling;
        glm::vec2 offset;
        float shininess;
    };
    MaterialUniform materialUniform{
		.color{1, 0, 0, 1},
        .tiling{1.0f, 1.0f},
        .offset{0.0f, 0.0f},
        .shininess{ 128.0f }
    };
    
    
    Transform transform;





    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());

    float scale = 0.5f;
    glm::vec4 color = { 1, 0, 1, 1 };


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
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_A)) { direction.x -= 1; }
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_D)) { direction.x += 1; }
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_W)) { direction.z += 1; }
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_S)) { direction.z -= 1; }

        vertexUniform.cameraPosition += direction * 5.0f * dt;


        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_1)) lightUniform.type = LightType::Directional;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_2)) lightUniform.type = LightType::Point;

        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_3))
        {
            lightUniform.type = LightType::Spot;
            lightUniform.position = glm::vec3{ 0, 20, 0 };
            lightUniform.innerCutoff = glm::cos(glm::radians(10.0f));
            lightUniform.outerCutoff = glm::cos(glm::radians(15.0f));
        }


      

        //TRANSFORM
        transform.rotation.y = time * 90;



        auto mxrx = glm::rotate(glm::mat4{ 1.0f }, time * 3.0f, glm::vec3(1.0f, -0.3f, 1.0f));
        auto mxry = glm::scale(glm::mat4{ 1.0f }, glm::vec3(3.0f, -0.5f, -0.3f));

        //VIEW MATRIX
        auto view = glm::lookAtRH(vertexUniform.cameraPosition, vertexUniform.cameraPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


        //PROJECTION MATRIX
        float aspectRatio = Engine::Instance().GetRenderer().GetWidth() / static_cast<float>(Engine::Instance().GetRenderer().GetHeight());
        auto projection = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 20.0f);

        //MVP 
		vertexUniform.model = transform.GetMatrix();
		vertexUniform.mvp = projection * view * vertexUniform.model;



        Engine::Instance().GetRenderer().BeginFrame();

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);
        Engine::Instance().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Instance().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
        Engine::Instance().GetRenderer().SetLightUniform(2, &materialUniform, sizeof(materialUniform));
        Engine::Instance().GetRenderer().SetFragmentUniform(0, &materialUniform, sizeof(materialUniform));

        Engine::Instance().GetRenderer().SetFragmentUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Instance().GetRenderer().SetFragmentUniform(1, &lightUniform, sizeof(lightUniform));
		//Engine::Instance().GetRenderer().SetFragmentTexture(0, *texture());


        
	

        model->Draw(Engine::Instance().GetRenderer());



        Engine::Instance().GetRenderer().EndFrame();




    }



    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

