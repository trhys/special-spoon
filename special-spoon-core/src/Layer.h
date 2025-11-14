#pragma once

#include "Core.h"
#include "SFML/Window/Event.hpp"

namespace Spoon 
{
	class Scene;

	class SPOON_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		void Init(Application* app);

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(sf::Time tick) {}
		virtual bool OnEvent(const sf::Event& e) { return false; }

		void RequestScene(std::string name, sf::Vector2f size);
		 
		template <typename T>
		void RequestEntity(std::string name, std::filesystem::path file_path)
		{
			p_App->GetSM()->CreateSceneEntity<T>(name, file_path);
		}
		// void BeginScene(std::string name);
		// void EndScene();
		// void PushOverlay(std::string name);
		// void PopOverlay();

	private:
		Application* p_App = nullptr;
	};
}