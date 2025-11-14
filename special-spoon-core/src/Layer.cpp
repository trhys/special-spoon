#include "Layer.h"
#include "Scene.h"

namespace Spoon
{
	void Layer::Init(Application* app)
	{
		p_App = app;
	}

	void Layer::RequestScene(std::string name, sf::Vector2f size)
	{
		Scene scene(name, size);
		p_App->GetSM()->CacheScene(scene);
	}
	
	// void Layer::BeginScene(std::string name)
	// {
	// 	p_App->GetSM()->ActivateScene(name);
	// }

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