#include "stdafx.h"
#include "Wiimote.h"

WiimoteBuzzerLib::Wiimote::Wiimote(const WiimoteScanner::WiimoteData & DeviceData)
	:DeviceHandle(DeviceData.DeviceHandle), ReadIo(nullptr), UseOutputReportSize(DeviceData.UsesToshibaStack)
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

	SetReportMode();

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

	WiimoteDisconnected(this, nullptr);

	FreeResources();
}

void WiimoteBuzzerLib::Wiimote::StopContinousReader()
{
	Abort = true;

	if ((ReadThread != nullptr) && (System::Threading::Thread::CurrentThread != ReadThread) && (ReadThread->IsAlive))
	{
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
}

void WiimoteBuzzerLib::Wiimote::SetLED(WiimoteLED LED)
{
	DataBuffer Buffer({ 0x11, (UCHAR)LED});

	Send(Buffer);
}

void WiimoteBuzzerLib::Wiimote::RumbleBriefly()
{
	DataBuffer Buffer({ 0x10, 0x01});
	Send(Buffer);

	System::Threading::Thread::Sleep(400);

	Buffer[1] = 0x00;
	Send(Buffer);
}

void WiimoteBuzzerLib::Wiimote::SetReportMode()
{
	DataBuffer Buffer({ 0x12, 0x00, 0x30 });
	Send(Buffer);
}

void WiimoteBuzzerLib::Wiimote::ContinousReader()
{
	InputReport Buffer = {};
	DWORD BytesRead;

	while (!Abort)
	{
		Buffer.fill(0);
		ResetEvent(ReadIo->hEvent);
		BytesRead = 0;

		BOOL Result = ReadFile(DeviceHandle, Buffer.data(), (DWORD)Buffer.size(), &BytesRead, ReadIo);
		if (!Result)
		{
			DWORD Error = GetLastError();
			if (Error != ERROR_IO_PENDING)
			{
				// Error
				Disconnect();
				break;
			}
			else
			{
				if (!GetOverlappedResult(DeviceHandle, ReadIo, &BytesRead, TRUE))
				{
					// Error
					Disconnect();
					break;
				}

				if (ReadIo->Internal == STATUS_PENDING)
				{
					CancelIo(DeviceHandle);
					continue;
				}
			}
		}

		CheckInput(Buffer);
	}
}

void WiimoteBuzzerLib::Wiimote::CheckInput(const InputReport & Input)
{
	if (Input[0] == 0x20)
	{
		SetReportMode();
		return;
	}

	if (Input[2] & 0x0C)
	{
		ButtonPressed(this, nullptr);
	}
}

void WiimoteBuzzerLib::Wiimote::Send(DataBuffer & Buffer)
{
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD BytesWritten;
	OVERLAPPED Overlapped = {};

	if ((UseOutputReportSize) && (Buffer.size() < WiimoteReportSize))
	{
		Buffer.resize(WiimoteReportSize, 0);
	}

	BOOL Result = WriteFile(DeviceHandle, Buffer.data(), (DWORD)Buffer.size(), &BytesWritten, &Overlapped);
	if (!Result)
	{
		DWORD Error = GetLastError();

		if (Error == ERROR_INVALID_USER_BUFFER)
		{
			SendFallback(Buffer);
		}

		if (Error != ERROR_IO_PENDING)
		{
			// Error
			return;
		}
	}

	if (!GetOverlappedResult(DeviceHandle, &Overlapped, &BytesWritten, TRUE))
	{
		// Error
		return;
	}
}

void WiimoteBuzzerLib::Wiimote::SendFallback(DataBuffer & Buffer)
{
	HidD_SetOutputReport(DeviceHandle, Buffer.data(), (ULONG)Buffer.size());
}