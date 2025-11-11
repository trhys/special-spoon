#include "Layer.h"

namespace Spoon
{
	void Layer::OnUpdate(sf::Time tick)
	{
		ProcessBuffer();
		m_SceneRoot.Update(tick, this);
	}

	sf::Texture& Layer::GetTexture(const std::string id, const std::filesystem::path file_path)
	{
		return p_RSM->AqcuireTexture(id, file_path);
	}

	void Layer::DrawScene(sf::RenderTarget& target, sf::RenderStates states)
	{
		m_SceneRoot.draw(target, states);
	}

	void Layer::ProcessBuffer()
	{
		for (auto& func : m_CreationBuffer)
		{
			Node* new_node = func();
			if (new_node)
			{
				AddSceneNode(std::move(new_node));
			}
		}

		m_CreationBuffer.clear();
	}
}