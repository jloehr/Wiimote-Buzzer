#include "stdafx.h"
#include "WiimoteHub.h"

WiimoteBuzzerLib::WiimoteHub::WiimoteHub()
	:Scanner(new WiimoteScanner())
{

}

WiimoteBuzzerLib::WiimoteHub::~WiimoteHub()
{
	delete Scanner;
}

void WiimoteBuzzerLib::WiimoteHub::StartScanning()
{
	if (ScannerThread != nullptr)
	{
		return;
	}

	System::Threading::ThreadStart^ ThreadEntry = gcnew System::Threading::ThreadStart(this, &WiimoteHub::ScannerThreadEntry);
	ScannerThread = gcnew System::Threading::Thread(ThreadEntry);
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
		Scanner->ScanForWiimotes();
		//Check for new ones
		//Raise Event

		System::Threading::Thread::Sleep(1000);
	}
}