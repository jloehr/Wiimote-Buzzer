#include "stdafx.h"
#include "Wiimote.h"

WiimoteBuzzerLib::Wiimote::Wiimote(const WiimoteScanner::WiimoteData & DeviceData)
{
	this->DeviceInstanceId = DeviceData.DeviceInstanceId;
}

WiimoteBuzzerLib::Wiimote::~Wiimote()
{}