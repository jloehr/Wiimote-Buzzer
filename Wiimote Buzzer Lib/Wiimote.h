#pragma once

#include "WiimoteScanner.h"

namespace WiimoteBuzzerLib {

	public ref class Wiimote
	{
	public:
		Wiimote(const WiimoteScanner::WiimoteData & DeviceData);
		~Wiimote();
		property DWORD DeviceInstanceId;

		event System::EventHandler<System::EventArgs^>^ ButtonPressed;
		event System::EventHandler<System::EventArgs^>^ WiimoteDisconnected;
	};
}

