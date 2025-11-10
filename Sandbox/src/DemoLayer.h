#pragma once

#include "Spoon.h"
#include <functional>

class DemoLayer : public Spoon::Layer
{
public:
    DemoLayer()
    ~DemoLayer() {}

    void OnAttach() override;
    void OnUpdate(sf::Time tick) override;
    bool OnEvent(const sf::Event& e) override;

    // Scene controls
    void AddSceneNode(Node child);
    void DrawScene(sf::RenderTarget& target, sf::RenderStates states) override;
    void ShowScene() { scene_IsActive = true; }
    void HideScene() { scene_IsActive = false; }

    // Create a new node of type "node_type"
    // The request goes to the m_CreationQueue buffer 
    // and executes during OnUpdate
    template <typename node_type, typename... Args>
    void CreateNode(Args&&...args)
    {
        m_CreationBuffer.push_back([=]() mutable 
        {
            return new node_type(std::forward<Args>args);
        });
    }

    void ProcessBuffer()
    {
        for(auto& func : m_CreationBuffer)
        {
            Node* new_node = func();
            if(new_node)
            {
                AddSceneNode(std::move(new_node));
            }
        }

        m_CreationBuffer.clear();
    }

private:
    Scene m_SceneRoot;

    std::function<Node*> m_CreationBuffer;

    bool scene_IsActive = false;
};