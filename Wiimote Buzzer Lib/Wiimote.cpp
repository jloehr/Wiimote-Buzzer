#include "stdafx.h"
#include "Wiimote.h"

WiimoteBuzzerLib::Wiimote::Wiimote(const WiimoteScanner::WiimoteData & DeviceData)
	:DeviceHandle(DeviceData.DeviceHandle), ReadIo(nullptr)
{
	this->DeviceInstanceId = DeviceData.DeviceInstanceId;
}

WiimoteBuzzerLib::Wiimote::~Wiimote()
{
	Disconnect();
}

void WiimoteBuzzerLib::Wiimote::StartContinousReader()
{
	if (ReadThread != nullptr)
	{
		return;
	}

	//DataBuffer Buffer({ 0x12, 0x00, 0x30 });
	//Send(Buffer);

	Abort = false;

	ReadIo = new OVERLAPPED();
	ReadIo->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	System::Threading::ThreadStart^ ThreadEntry = gcnew System::Threading::ThreadStart(this, &Wiimote::ContinousReader);
	ReadThread = gcnew System::Threading::Thread(ThreadEntry);
	ReadThread->Name = "Wiimote Continous Reader";

	ReadThread->Start();
}

void WiimoteBuzzerLib::Wiimote::Disconnect()
{
	StopContinousReader();

	FreeResources();
}

void WiimoteBuzzerLib::Wiimote::StopContinousReader()
{
	if ((ReadThread != nullptr) && (ReadThread->IsAlive))
	{
		Abort = true;
		do {
			SetEvent(ReadIo->hEvent);
		} while (!ReadThread->Join(100));
	}
}

void WiimoteBuzzerLib::Wiimote::FreeResources()
{
	if (ReadIo != nullptr)
	{
		if (ReadIo->hEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(ReadIo->hEvent);
			ReadIo->hEvent = INVALID_HANDLE_VALUE;
		}

		delete ReadIo;
		ReadIo = nullptr;
	}

	CloseHandle(DeviceHandle);
	DeviceHandle = INVALID_HANDLE_VALUE;

	ReadThread = nullptr;

	WiimoteDisconnected(this, nullptr);
}

void WiimoteBuzzerLib::Wiimote::ContinousReader()
{
	UCHAR Buffer[22];
	DWORD BytesRead;

	while (!Abort)
	{
		ZeroMemory(Buffer, sizeof(Buffer));
		ResetEvent(ReadIo->hEvent);
		BytesRead = 0;

		BOOL Result = ReadFile(DeviceHandle, &Buffer, sizeof(Buffer), &BytesRead, ReadIo);
		if (!Result)
		{
			DWORD Error = GetLastError();
			if (Error != ERROR_IO_PENDING)
			{
				// Error
				FreeResources();
				break;
			}
			else
			{
				if (!GetOverlappedResult(DeviceHandle, ReadIo, &BytesRead, TRUE))
				{
					// Error
					FreeResources();
					break;
				}

				if (ReadIo->Internal == STATUS_PENDING)
				{
					CancelIo(DeviceHandle);
					continue;
				}
			}
		}

		// Check for Input
	}
}

