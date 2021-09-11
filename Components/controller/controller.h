#ifndef CONTROLLER_CONTROLLER_H_
#define CONTROLLER_CONTROLLER_H_

#include <core/engine.h>
#include <core/machine.h>
#include <core/task.h>
#include <console/controller.h>

MACHINE(device, Controller)

	M_TASK(timeout,{SM_POST(Event::Timeout);})

	U_TEXT(100, data2Server)

public:
	void init();

private:
//	STATE_DEF(StartUp)

private:
	enum class Event{Timeout};

MACHINE_END

#endif /* CONTROLLER_CONTROLLER_H_ */
