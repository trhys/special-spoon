#include "DemoScene.h"

DemoScene::DemoScene()
{

}

void DemoScene::SetScene()
{
    m_Texture = m_RSM->m_Textures["demoscene"];
    m_Sprite.setTexture(m_Texture);
}

void DemoScene::OnDraw(sf::RenderTarget& target, const sf::Transform& transform) const
{
    target.draw(m_Sprite, transform);
}

void DemoScene::SpawnZombie(ResourceManager& rsm)
{
    m_Children.emplace_back(new DemoZombie(rsm));
}