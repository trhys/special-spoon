#include "Layer.h"
#include "ResourceManager.h"
#include "Scene/SceneManager.h"

namespace Spoon
{
	void Layer::Init(SceneManager* sm)
	{
		p_SM = sm;
	}

	void Layer::RequestScene(std::string name, Scene* scene)
	{
		p_SM->CacheScene(name, scene);
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
