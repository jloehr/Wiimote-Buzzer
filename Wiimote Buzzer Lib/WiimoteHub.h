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

		void DisconnectWiimotes();

		event System::EventHandler<Wiimote^>^ NewWiimoteFound;

	private:
		DeviceInstanceIdSet * WiimotesInUse;
		WiimoteScanner * Scanner;

		System::Threading::Thread^ ScannerThread;
		volatile bool Abort;

		System::Collections::Generic::List<Wiimote^>^ ConnectedWiimotes;
		Object^ WiimotesLock;

		void ScannerThreadEntry();
		void OnWiimoteDisconnected(System::Object ^sender, System::EventArgs ^e);
	};
}
