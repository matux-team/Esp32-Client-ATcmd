#include <esp32/esp-event.h>
#include <esp32/driver.h>
#include <esp32/hal.h>

esp32::EspEvent::EspEvent(core::Component* component, Handler handler):
	component_(component),
	handler_(handler)
{
	index_ = Driver::instance().registerEvent_(this);
}

void esp32::EspEvent::post(uint8_t* data, uint16_t length)
{
	Driver::instance().pushCommand_(index_, length, data);
}

void esp32::EspEvent::execute(bool ok, uint8_t* data, uint16_t length)
{
	(component_->*handler_)(ok, data, length);
}
