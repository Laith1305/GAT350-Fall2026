#pragma once
#include "Resources/Resource.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "IndexBuffer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include "MaterialInstance.h"


// forward declare assimp types so the header doesn't pull in assimp
struct aiNode;
struct aiMesh;
struct aiScene;

namespace nu
{
	class Model : public Resource
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec2 texcoord;
			glm::vec3 normal;
			glm::vec3 tangent;
		};

		// each mesh owns its own vertex and index buffer
		struct Mesh
		{
			std::unique_ptr<VertexBuffer> vertexBuffer;
			std::unique_ptr<IndexBuffer> indexBuffer;
			uint32_t materialIndex = 0;
		};

	public:
		bool Load(const std::string& filename, const class Renderer& renderer);

		// binds and draws every mesh, call between BeginFrame and EndFrame with a pipeline set
		void Draw(const class Renderer& renderer, const std::vector<res_t<class MaterialInstance>>& overrides = {}) const;

		

	private:
		void ProcessNode(aiNode* node, const aiScene* scene, const class Renderer& renderer);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, const class Renderer& renderer);

		void LoadMaterials(const aiScene* scene, const std::string& directory, const Renderer& renderer);



	private:
		// a model can be made up of multiple meshes
		std::vector<Mesh> m_meshes;
		std::vector<MaterialInstance> m_materials;
	};
}
