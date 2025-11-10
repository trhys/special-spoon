#pragma once

#include "Core.h"

namespace Spoon 
{
	class SPOON_API Layer
	{
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(sf::Time tick) {}
		virtual bool OnEvent(const sf::Event& e) { return false; }
		
		// Get rsm pointer to pass to scene when loading assets - > SetScene()
		// Gets called when pushed to stack
		void GetRSM(ResourceManager* rsm) { p_RSM = rsm; }

		sf::Texture& GetTexture(const std::string id, const std::filesystem::path file_path)
		{
			p_RSM->AqcuireTexture(const std::string id, const std::filesystem::path file_path);
		}

		virtual void DrawScene(sf::RenderTarget& target, sf::RenderStates states) {}
		
	private:
		ResourceManager* p_RSM;
	};
}