#pragma once

#include "WiimoteScanner.h"

typedef std::vector<UCHAR> DataBuffer;

namespace WiimoteBuzzerLib {

	public ref class Wiimote
	{
	public:
		enum class WiimoteLED { LED1 = 0x10, LED2 = 0x20, LED3 = 0x30, LED4 = 0x40 };

		Wiimote(const WiimoteScanner::WiimoteData & DeviceData);
		~Wiimote();

		void StartContinousReader();
		void Disconnect();

		void SetLED(WiimoteLED LED);
		void RumbleBriefly();

		property DWORD DeviceInstanceId;

		event System::EventHandler^ ButtonPressed;
		event System::EventHandler^ WiimoteDisconnected;
	private:
		static const int WiimoteReportSize = 22;

		System::Threading::Thread^ ReadThread;
		volatile bool Abort;

		HANDLE DeviceHandle;
		LPOVERLAPPED ReadIo;
		bool UseOutputReportSize;

		void ContinousReader();
		void StopContinousReader();

		void FreeResources();

		void Send(DataBuffer & Buffer);
		void SendFallback(DataBuffer & Buffer);
	};
}

