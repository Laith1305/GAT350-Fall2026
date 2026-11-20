#pragma once
#include "Actor.h"
#include "Renderer/Renderer.h"
#include "Editor/Editor.h"
#include <vector>
#include <string>
#include <memory>

namespace nu
{
	class Scene : public IGui
	{
	public:
		void Update(float dt);
		void Draw(const class Renderer& renderer);

		void AddActor(std::unique_ptr<Actor> actor);
		void RemoveAllActors(bool force = false);

		bool Load(const std::string& sceneName);

		void UpdateGui() override;

		template<typename T = Actor>
		T* GetActorByName(const std::string& name);

		template<typename T = Actor>
		std::vector<T*> GetActorsByTag(const std::string& tag);

		const auto& GetActors() const { return m_actors; }

		void SetGame(class Game* game) { m_game = game; }
		class Game* GetGame() { return m_game; }

	private:
		std::vector<std::unique_ptr<Actor>> m_actors;
		std::vector<std::unique_ptr<Actor>> m_pendingActors;

		class Game* m_game{ nullptr };

		glm::vec3 m_ambientLight{ 0.2f };
	};

	
	template<typename T>
	inline T* Scene::GetActorByName(const std::string& name)
	{
		for (auto& actor : m_actors)
		{
			T* actorT = dynamic_cast<T*>(actor.get());
			if (actorT && actorT->m_name == name)
			{
				return actorT;
			}
		}

		return nullptr;
	}

	template<typename T>
	inline std::vector<T*> Scene::GetActorsByTag(const std::string& tag)
	{
		std::vector<T*> result;
		for (auto& actor : m_actors)
		{
			T* actorT = dynamic_cast<T*>(actor.get());
			if (actorT && actorT->m_tag == tag)
			{
				result.push_back(actorT);
			}
		}
		return result;
	}
}