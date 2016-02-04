#pragma once

#include "WiimoteScanner.h"

typedef std::vector<UCHAR> DataBuffer;

namespace WiimoteBuzzerLib {

	public ref class Wiimote
	{
	public:
		Wiimote(const WiimoteScanner::WiimoteData & DeviceData);
		~Wiimote();

		void StartContinousReader();
		void Disconnect();

		property DWORD DeviceInstanceId;

		event System::EventHandler^ ButtonPressed;
		event System::EventHandler^ WiimoteDisconnected;
	private:
		System::Threading::Thread^ ReadThread;
		volatile bool Abort;

		HANDLE DeviceHandle;
		LPOVERLAPPED ReadIo;

		void ContinousReader();
		void StopContinousReader();

		void FreeResources();
	};
}

