#include "Layer.h"

void Layer::OnAttach()
{
	m_Window.create(sf::VideoMode({ 600, 600 }), "Sandbox Test"); // CURRENTLY A TEST LAYER
}