#include "stdafx.h"
#include "WiimoteHub.h"

WiimoteBuzzerLib::WiimoteHub::WiimoteHub()
	:WiimotesInUse(new DeviceInstanceIdSet()),
	WiimotesLock(gcnew Object()),
	Scanner(new WiimoteScanner(*WiimotesInUse)),
	ConnectedWiimotes(gcnew System::Collections::Generic::List<Wiimote^>)
{

}

WiimoteBuzzerLib::WiimoteHub::~WiimoteHub()
{
	StopScanning();

	System::Threading::Monitor::Enter(WiimotesLock);
	delete Scanner;
	Scanner = nullptr;
	delete WiimotesInUse;
	WiimotesInUse = nullptr;
	System::Threading::Monitor::Exit(WiimotesLock);
}

void WiimoteBuzzerLib::WiimoteHub::StartScanning()
{
	if (ScannerThread != nullptr)
	{
		return;
	}

	System::Threading::ThreadStart^ ThreadEntry = gcnew System::Threading::ThreadStart(this, &WiimoteHub::ScannerThreadEntry);
	ScannerThread = gcnew System::Threading::Thread(ThreadEntry);
	ScannerThread->Name = "Scanner Thread";
	Abort = false;

	ScannerThread->Start();
}

void WiimoteBuzzerLib::WiimoteHub::StopScanning()
{
	if (ScannerThread == nullptr)
	{
		return;
	}

	Abort = true;
	ScannerThread->Join();

	ScannerThread = nullptr;
}

void WiimoteBuzzerLib::WiimoteHub::ScannerThreadEntry()
{
	while (!Abort)
	{
		System::Threading::Monitor::Enter(WiimotesLock);
		WiimoteScanner::WiimoteDataVector NewWiimotes = Scanner->ScanForWiimotes();
	
		for (auto & NewDevice : NewWiimotes)
		{
			WiimotesInUse->insert(NewDevice.DeviceInstanceId);

			Wiimote^ NewWiimote = gcnew Wiimote(NewDevice);
			ConnectedWiimotes->Add(NewWiimote);
			NewWiimote->WiimoteDisconnected += gcnew System::EventHandler<System::EventArgs ^>(this, &WiimoteBuzzerLib::WiimoteHub::OnWiimoteDisconnected);
			
			NewWiimoteFound(this, NewWiimote);
		}

		System::Threading::Monitor::Exit(WiimotesLock);
		System::Threading::Thread::Sleep(1000);
	}
}

void WiimoteBuzzerLib::WiimoteHub::OnWiimoteDisconnected(System::Object ^sender, System::EventArgs ^e)
{
	Wiimote^ DisconnectedWiimote = (Wiimote^)sender;
	System::Threading::Monitor::Enter(WiimotesLock);

	WiimotesInUse->erase(DisconnectedWiimote->DeviceInstanceId);
	ConnectedWiimotes->Remove(DisconnectedWiimote);
	DisconnectedWiimote->WiimoteDisconnected -= gcnew System::EventHandler<System::EventArgs ^>(this, &WiimoteBuzzerLib::WiimoteHub::OnWiimoteDisconnected);

	System::Threading::Monitor::Exit(WiimotesLock);
}
