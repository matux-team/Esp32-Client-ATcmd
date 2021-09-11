#include "controller.h"
#include "core/engine.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <console/driver.h>

void console::Controller::init()
{
    console::Driver::instance().init();
    plotTask_.start(20);
}

void console::Controller::print(const char* text)
{
    int l = strlen(text) + 1;
    console::Driver::instance().sendPacket(console::Controller::MessageReceived, l, (uint8_t*)text);
}

bool console::Controller::sendOSC(uint8_t channel, uint8_t length, uint16_t index, const uint8_t* data)
{
    if (!console::Driver::instance().checkBeforeSend(length + 100)) return false;
    uint8_t buf[256];
    buf[0] = channel;
    buf[1] = (index >> 8) & 0xFF;
    buf[2] = index & 0xFF;

    for (int i =0;i < length;i++)
    {
        buf[i+3] = data[i];
    }
    console::Driver::instance().sendPacket(console::Controller::Oscilloscope, length+3, buf);
    return true;
}

void console::Controller::printf(const char* format, ...)
{
    char buf[256];
    va_list args;
    va_start (args, format);
    int l = vsprintf(buf, format ,args) + 1;
    va_end(args);
    console::Driver::instance().sendPacket(console::Controller::MessageReceived, l, (uint8_t*)buf);
}

console::Command* console::Controller::registerCommand(uint16_t type, Handler handler)
{
    Command* cmd = new Command();
    cmd->type = type;
    cmd->handler = handler;
    cmd->next = commands_;
    commands_ = cmd;
    return cmd;
}

void console::Controller::updateIntField(uint16_t command, int32_t value)
{
    uint8_t buf[6];
    ENCODE_16(command,buf,0);
    ENCODE_32(value,buf,2);
    console::Driver::instance().sendPacket(console::Controller::IntFieldReceived, 6, buf);
}

void console::Controller::processCommand(uint16_t type, uint8_t length, uint8_t* data)
{
	Command* it = commands_;
	while (it!=nullptr)
	{
		if (it->type==type)
		{
			(*it->handler)(type, length,data);
			break;
		}
		it=it->next;
	}
}

void console::Controller::updateTextField(uint16_t command, char* data)
{
    int8_t length = strlen(data);
    updateTextField(command, length, (uint8_t*)data);
}

void console::Controller::updateTextField(uint16_t command, uint8_t length, uint8_t* data)
{
    if (length > MAX_PACKET_LENGTH-6) return;
    uint8_t buf[length+2];
    ENCODE_16(command,buf,0);
    for (int i=0;i<length;i++) buf[i+2] = data[i];
    console::Driver::instance().sendPacket(console::Controller::TextFieldReceived, length+2, buf);
}
