#include "DemoLayer.h"
#include "Background.h"
#include "DemoZombie.h"

DemoLayer::DemoLayer()
{
    AddSceneNode(new Background(Spoon::Layer::GetTexture("background", "resources/TD-Scene,png")));
    AddSceneNode(new ZombieSpawner({200, 200}));
}

void DemoLayer::OnAttach()
{

}

void DemoLayer::OnUpdate(sf::Time tick)
{
    ProcessBuffer();
    
    m_SceneRoot.Update(tick, this);
}

bool DemoLayer::OnEvent(const sf::Event& e)
{
    if(const auto& keypress = e.getIf<sf::Event::KeyPressed>())
    {
        if (keypress->code == sf::Keyboard::Key::Enter)
        {
            if(scene_IsActive) { HideScene(); }
            else if(!scene_IsActive) { ShowScene(); }
        }
    }

    return false;
}

DemoLayer::AddSceneNode(Node child)
{
    m_SceneRoot.AddChildNode(child);
}

void DemoScene::DrawScene(sf::RenderTarget& target, sf::RenderStates states)
{
    if(scene_IsActive)
    {
        target.draw(m_SceneRoot, states);
    }
}