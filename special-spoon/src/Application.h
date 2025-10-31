#pragma once

#include <Core.h>
//#include <SFML/Graphics.hpp>

namespace Spoon { 

	class SPOON_API Application
	{
	public:
		Application() {};
		virtual ~Application() {};

		void Run();

	private:
	
	};

	Application* CreateApp();

}