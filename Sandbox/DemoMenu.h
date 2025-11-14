#pragma once

#include "Layer.h"

class DemoMenu : public Spoon::Layer
{
public:
	DemoMenu() {};
	~DemoMenu() {};

	bool OnEvent(sf::Event& e);
};

