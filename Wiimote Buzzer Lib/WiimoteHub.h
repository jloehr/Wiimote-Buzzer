#pragma once

#include "Wiimote.h"
#include "WiimoteScanner.h"

namespace WiimoteBuzzerLib {

	public ref class WiimoteHub
	{
	public:
		WiimoteHub();
		~WiimoteHub();

		void StartScanning();
		void StopScanning();

		event System::EventHandler<Wiimote^>^ NewWiimoteFound;

	private:
		WiimoteScanner * Scanner;
		System::Threading::Thread^ ScannerThread;
		volatile bool Abort;

		void ScannerThreadEntry();
	};
}
