#pragma once

#include "Core.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "SFML/Graphics.hpp"

#include <functional>
#include <vector>

namespace Spoon 
{
	class ResourceManager;
	class PhysicsManager;

	class SPOON_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		void Init(ResourceManager* rsm, PhysicsManager* pm);

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(sf::Time tick);
		virtual bool OnEvent(const sf::Event& e) { return false; }

		// Scene controls
		sf::Texture& GetTexture(const std::string id, const std::filesystem::path file_path);
		void DrawScene(sf::RenderTarget& target, sf::RenderStates states);
		void ShowScene() { scene_IsActive = true; }
		void HideScene() { scene_IsActive = false; }
		bool GetIsActive() { return scene_IsActive; }
		void SetSceneBounds(sf::Vector2f position, sf::Vector2f size) { m_SceneRoot.SetBounds(position, size); }

		void Physics();

		// Scene child node creation
		template <typename node_type, typename... Args>
		void CreateNode(Args&&...args)
		{
			m_CreationBuffer.emplace_back([=]() mutable
				{
					m_SceneRoot.AddChildNode(new node_type(std::forward<Args>(args)...));
				});
		}
		void ProcessBuffer();
		
	private:
		ResourceManager* p_RSM = nullptr;
		PhysicsManager* p_PM = nullptr;

		Scene m_SceneRoot;
		bool scene_IsActive = false;

		std::vector<std::function<void()>> m_CreationBuffer;
	};
}
