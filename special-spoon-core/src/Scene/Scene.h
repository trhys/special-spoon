#pragma once

#include "Core.h"
#include "Node.h"

namespace Spoon 
{
    class ResourceManager;

    class SPOON_API Scene : public Node
    {
    public:
        Scene() {}
        Scene(std::string name, sf::Vector2f size) : m_Name(name) { m_Bounds.size = size; }
        virtual ~Scene() {}

        virtual void OnTransition() {}
        virtual void OnCache() {}
        virtual void OnStart() {}
        virtual void OnEnd() {}

        void Init(ResourceManager* rm);
        void LoadTexture(std::string id, std::filesystem::path file_path);
        void LoadFont(std::string id, std::filesystem::path file_path);
        void ShowScene();
        void HideScene();
        bool IsActive();

        std::string GetName() const { return m_Name; }
        sf::FloatRect GetBounds() const { return m_Bounds; }
        sf::View& GetView() { return mainview; }

        std::vector<Node*>& GetCollidablesGraph();

    private:
        void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const override { target.setView(mainview); }

        bool m_IsActive = false;
        ResourceManager* p_RM = nullptr;

        sf::View mainview;
        sf::FloatRect m_Bounds;
        std::string m_Name;

        std::vector<Node*> m_CollidablesGraph;
    };
}