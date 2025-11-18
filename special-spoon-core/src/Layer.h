#pragma once

#include "Core.h"
#include "Scene/SceneManager.h"

#include "SFML/Window/Event.hpp"
#include "SFML/System/Time.hpp"

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
		void BeginScene(std::string name);
		void EndScene();
		// void PushOverlay(std::string name);
		// void PopOverlay();

		SceneManager* GetSM() { return p_SM; }

	private:
		SceneManager* p_SM = nullptr;
		ResourceManager* p_RM = nullptr;
	};
}