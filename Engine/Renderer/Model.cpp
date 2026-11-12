#include "pch.h"
#include "Model.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Engine.h>
#include "Core/File.h"

#define DEFAULT_MATERIAL "Materials/phong.material.json"

namespace nu
{
	bool Model::Load(const std::string& filename, const Renderer& renderer)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Could not load assimp file " << filename << ": " << importer.GetErrorString() << std::endl;

			return false;
		}

		// texture paths in the model are relative to the model file
		std::string directory = GetParentDirectory(filename) + "/";
		LoadMaterials(scene, directory, renderer);
		LoadMaterials(scene, directory, renderer);



		ProcessNode(scene->mRootNode, scene, renderer);

		return true;
	}

	

	void Model::Draw(const Renderer& renderer, const std::vector<res_t<class MaterialInstance>>& overrides) const
	{
		for (const auto& mesh : m_meshes)
		{
			uint32_t slot = mesh.materialIndex;
			MaterialInstance material = slot < overrides.size() && overrides[slot] ? *overrides[slot] : m_materials[slot];

			renderer.SetMaterial(material);

			renderer.SetVertexBuffer(*mesh.vertexBuffer);
			renderer.SetIndexBuffer(*mesh.indexBuffer);
			renderer.DrawIndex(*mesh.indexBuffer);
		}
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene, const Renderer& renderer)
	{
		// process the current node meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, renderer);
		}

		// process the current node children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, renderer);
		}
	}

	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const Renderer& renderer)
	{
		// get model vertex attributes
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.position = glm::vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			vertex.texcoord = (mesh->mTextureCoords[0])
				? glm::vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y }
			: glm::vec2{ 0, 0 };

			vertex.normal = (mesh->mNormals)
				? glm::vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z }
			: glm::vec3{ 0, 0, 0 };

			vertex.tangent = (mesh->mTangents)
				? glm::vec3{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z }
			: glm::vec3{ 0, 0, 0 };

			vertices.push_back(vertex);
		}

		// get model indices
		std::vector<uint32_t> indices;
		indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// create the vertex buffer (uploads to the gpu)
		Mesh newMesh;
		newMesh.vertexBuffer = std::make_unique<VertexBuffer>();
		if (!newMesh.vertexBuffer->Create(vertices, renderer))
		{
			std::cerr << "Could not create vertex buffer for mesh " << mesh->mName.C_Str() << std::endl;

			return;
		}

		// create the index buffer (uploads to the gpu)
		newMesh.indexBuffer = std::make_unique<IndexBuffer>();
		if (!newMesh.indexBuffer->Create(indices, renderer))
		{
			std::cerr << "Could not create index buffer for mesh " << mesh->mName.C_Str() << std::endl;

			return;
		}

		newMesh.materialIndex = mesh->mMaterialIndex;

		m_meshes.push_back(std::move(newMesh));
	}
	void Model::LoadMaterials(const aiScene* scene, const std::string& directory, const Renderer& renderer)
	{
		// every imported material starts from the standard phong material
		auto material = Resources().Get<Material>(DEFAULT_MATERIAL, renderer);

		std::cout << "Loading " << scene->mNumMaterials << " material(s)" << std::endl;

		m_materials.reserve(scene->mNumMaterials);
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			const aiMaterial* source = scene->mMaterials[i];
			MaterialInstance& instance = m_materials.emplace_back(material);

			// base color: gltf uses BASE_COLOR, older formats (obj, fbx) use DIFFUSE
			aiColor4D color;
			if (source->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS ||
				source->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				instance.GetData().baseColor = glm::vec4{ color.r, color.g, color.b, color.a };
			}

			// shininess varies a lot between exporters, ignore zero
			float shininess;
			if (source->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS && shininess > 0.0f)
			{
				instance.GetData().shininess = shininess;
			}

			// base texture
			aiString path;
			if (source->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS ||
				source->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				// embedded textures (glb, some fbx) use paths like "*0" and aren't loaded from disk yet
				if (path.C_Str()[0] == '*')
				{
					std::cerr << "Embedded textures not supported yet: " << path.C_Str() << std::endl;
				}
				else
				{
					auto texture = Resources().Get<Texture>(directory + path.C_Str(), renderer);
					if (!instance.SetTexture("base_map", texture))
					{
						std::cerr << "Material has no base_map slot for texture: " << path.C_Str() << std::endl;
					}
				}
			}

			// print what was loaded for this slot
			aiString name;
			source->Get(AI_MATKEY_NAME, name);

			const MaterialData& data = instance.GetData();
			std::cout << "  [" << i << "] " << name.C_Str()
				<< " | color: " << data.baseColor.r << ", " << data.baseColor.g << ", " << data.baseColor.b << ", " << data.baseColor.a
				<< " | shininess: " << data.shininess
				<< " | texture: " << (path.length > 0 ? path.C_Str() : "none")
				<< std::endl;
		}
	}
}
