#pragma once

#include "TestEntity.h"

class TestLayer : public Spoon::Layer
{
public:
   TestLayer() {}
   virtual ~TestLayer() {}

   //void OnAttach() override;
   bool OnEvent(const sf::Event& e) override;
};