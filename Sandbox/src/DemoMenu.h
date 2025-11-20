#pragma once

#include "Spoon.h"

class DemoMenu : public Spoon::Layer
{
public:
	DemoMenu() {};
	~DemoMenu() {};

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(sf::Time tick) override;
	bool OnEvent(const sf::Event& e) override;

private:
	sf::Time timer;
	bool transitiontimer = false;
};