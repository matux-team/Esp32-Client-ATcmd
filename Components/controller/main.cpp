#include <controller/controller.h>

int main()
{
	COMPONENT_REG(device, Controller);
	core::Engine::instance().run();
	return 0;
}

