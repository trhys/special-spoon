#pragma once

#include "Core.h"
#include "ResourceManager.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "SFML/Graphics.hpp"

#include <functional>
#include <vector>

namespace Spoon 
{
	class Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(sf::Time tick);
		virtual bool OnEvent(const sf::Event& e) { return false; }

		void GetRSM(ResourceManager* rsm) { p_RSM = rsm; }
		sf::Texture& GetTexture(const std::string id, const std::filesystem::path file_path);

		void DrawScene(sf::RenderTarget& target, sf::RenderStates states);
		void ShowScene() { scene_IsActive = true; }
		void HideScene() { scene_IsActive = false; }
		bool GetIsActive() { return scene_IsActive; }

		template <typename T>
		void AddSceneNode(T child)
		{
			m_SceneRoot.AddChildNode(child);
		}

		template <typename node_type, typename... Args>
		void CreateNode(Args&&...args)
		{
			m_CreationBuffer.push_back([=]() mutable
				{
					return new node_type(args);
				});
		}

		void ProcessBuffer();
		
	private:
		ResourceManager* p_RSM;

		Scene m_SceneRoot;
		bool scene_IsActive = false;

		std::vector<std::function<Node*()>> m_CreationBuffer;
	};
}