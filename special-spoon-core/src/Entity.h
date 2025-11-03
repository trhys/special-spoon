#include "SFML/Graphics.hpp"

namespace Spoon
{
    class Entity : public sf::Drawable
    {
    public:
        Entity() {}
        Entity(sf::Texture texture) { m_Sprite.setTexture(texture); }
        virtual ~Entity() {}

        virtual void OnAdd() {};
        virtual void OnKill() {};

    private:
        sf::Sprite m_Sprite;
        sf::Texture m_Texture({300,300});
    }
}