#include "Layer.h"
#include "ResourceManager.h"
#include "Physics/PhysicsManager.h"

namespace Spoon
{
	void Layer::Init(ResourceManager* rsm, PhysicsManager* pm)
	{
		p_RSM = rsm;
		p_PM = pm;
	}

	void Layer::OnUpdate(sf::Time tick)
	{
		ProcessBuffer();
		UpdateScene(tick, this);
	}

	sf::Texture& Layer::GetTexture(const std::string id, const std::filesystem::path file_path)
	{
		return p_RSM->AqcuireTexture(id, file_path);
	}

	void Layer::UpdateScene(sf::Time tick, Layer* context)
	{
		m_SceneRoot.Update(tick, this);
	}

	void Layer::DrawScene(sf::RenderTarget& target, sf::RenderStates states)
	{
		m_SceneRoot.draw(target, states);
	}

	void Layer::Physics()
	{
		p_PM->CheckCollision(m_SceneRoot);
	}

	void Layer::ProcessBuffer()
	{
		for (auto& func : m_CreationBuffer)
		{
			func();
		}
		m_CreationBuffer.clear();
	}
}