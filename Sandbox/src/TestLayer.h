#include "Spoon.h"

class TestLayer : public Spoon::Layer
{
public:
    TestLayer() {}
    virtual ~Testlayer() {}

    virtual void OnAttach() override;
    virtual void OnEvent(sf::Event& e) override; 
}