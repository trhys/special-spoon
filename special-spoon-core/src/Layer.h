#pragma once

#include "Core.h"
#include "Scene/SceneManager.h"
#include "ResourceManager.h"

#include "SFML/Window/Event.hpp"
#include "SFML/System/Time.hpp"

#include <filesystem>

namespace Spoon 
{
	class SPOON_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		void Init(SceneManager* sm);

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(sf::Time tick) {}
		virtual bool OnEvent(const sf::Event& e) { return false; }

		void RequestScene(std::string name, Scene* scene);
		 
		// template <typename T>
		// void RequestEntity(std::string name, std::filesystem::path file_path)
		// {
		// 	T* entity = new T(p_RM->LoadTexture(name, file_path));
		// 	p_SM->CreateSceneEntity(entity);
		// }

		void BeginScene(std::string name);
		void EndScene();
		// void PushOverlay(std::string name);
		// void PopOverlay();

	private:
		SceneManager* p_SM = nullptr;
		ResourceManager* p_RM = nullptr;
	};
}