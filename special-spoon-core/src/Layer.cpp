#include "Entity.h"
#include "Layer.h"
#include "ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

namespace Spoon
{
	void Layer::Init(SceneManager* sm, ResourceManager* rm)
	{
		p_SM = sm;
		p_RM = rm;
	}

	void Layer::RequestScene(std::string name, sf::Vector2f size)
	{
		Scene scene(name, size);
		p_SM->CacheScene(scene);
	}
	
	 void Layer::BeginScene(std::string name)
	 {
	 	p_SM->ActivateScene(name);
	 }

	// void Layer::EndScene()
	// {
	// 	p_App->GetSM()->DeactivateScene();
	// }

	// void Layer::PushOverlay(std::string name)
	// {
	// 	p_App->GetSM()->ActivateOverlay(name);
	// }

	// void Layer::PopOverlay()
	// {
	// 	p_App->GetSM()->DeactivateOverlay();
	// }
}
