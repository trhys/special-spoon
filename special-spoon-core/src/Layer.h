#pragma once

#include "Core.h"
#include "Scene/Scene.h"

namespace Spoon 
{
	class ResourceManager;
	 
	class SPOON_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach(ResourceManager* rsm) { m_RSM = rsm; }
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual bool OnEvent(const sf::Event& e) { return false; }

		void LoadScene();
		void UnloadScene() { m_Scene = nullptr; }
		void SetScene(Scene* scene) { m_Scene = scene; }

		void DrawScene(sf::RenderTarget& target, const sf::Transform& scene_transform);

	private:
		Scene* m_Scene = nullptr;
		ResourceManager* m_RSM = nullptr;

	};
}