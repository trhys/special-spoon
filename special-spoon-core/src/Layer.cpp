#include "Layer.h"
#include "ResourceManager.h"
#include "Scene/SceneManager.h"

namespace Spoon
{
	void Layer::Init(SceneManager* sm, ResourceManager* rm)
	{
		p_SM = sm;
		p_RM = rm;
	}

	void Layer::RequestScene(std::string name, sf::Vector2f size)
	{
		p_SM->CacheScene(name, size);
	}
	
	 void Layer::BeginScene(std::string name)
	 {
	 	p_SM->ActivateScene(name);
	 }

	void Layer::EndScene()
	{
		p_SM->DeactivateScene();
	}

	// void Layer::PushOverlay(std::string name)
	// {
	// 	p_SM()->ActivateOverlay(name);
	// }

	// void Layer::PopOverlay()
	// {
	// 	p_SM()->DeactivateOverlay();
	// }
}
