#ifndef ESP32_ESP_EVENT_H_
#define ESP32_ESP_EVENT_H_

#include <core/base.h>

namespace esp32
{

class EspEvent
{
public:
    typedef void (core::Component::*Handler) (bool ok, uint8_t* data, uint16_t length);
    EspEvent(core::Component* component, Handler handler);
    void execute(bool ok, uint8_t* data, uint16_t length);
    void post(uint8_t* data, uint16_t length);
    uint8_t index(){return index_;}
protected:
    uint8_t index_;
private:
    core::Component *component_ = nullptr;
    Handler handler_;
};

}


#define ESP_EVENT(name)\
public:\
	esp32::EspEvent name##EspEvent = esp32::EspEvent(this, (esp32::EspEvent::Handler)&CLASS::name##EspEventHandler_);\
private:\
	void name##EspEventHandler_(bool ok, uint8_t* data, uint16_t length);

#define ESP_EVENT_HANDLER(cls,name) void cls::name##EspEventHandler_(bool ok, uint8_t* data, uint16_t length)




#endif /* ESP32_ESP_EVENT_H_ */
