#pragma once

#include "SFML\Graphics.hpp"

class Layer
{
public:
	Layer() {}
	~Layer() {}

	void OnAttach();
	//void OnDetach();
	//void OnUpdate();
	//void OnEvent(sf::Event& e);

	sf::RenderWindow& GetWindow() { return m_Window; }

private:
	sf::RenderWindow m_Window;

};