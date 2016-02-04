#include "stdafx.h"
#include "WiimoteScanner.h"


WiimoteScanner::WiimoteScanner(DeviceInstanceIdSet & WiimotesInUse)
	:WiimotesInUse(WiimotesInUse), DeviceInfoSet(NULL)
{
	HidD_GetHidGuid(&HidGuid);
}

WiimoteScanner::WiimoteDataVector WiimoteScanner::ScanForWiimotes()
{
	DeviceInfoSet = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (!DeviceInfoSet)
	{
		return WiimoteDataVector();
	}
	
	NewWiimotes.clear();

	DWORD DeviceIndex = 0;

	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {};
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);


	while (SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL, &HidGuid, DeviceIndex, &DeviceInterfaceData))
	{
		DeviceIndex++;
		CheckEnumeratedDeviceInterface(&DeviceInterfaceData);
	}

	return NewWiimotes;
}

void WiimoteScanner::CheckEnumeratedDeviceInterface(PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData)
{
	BOOL Result;
	DWORD RequiredSize;
	SP_DEVINFO_DATA DeviceInfoData = {};

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	Result = SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData, NULL, 0, &RequiredSize, NULL);

	PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(RequiredSize);
	DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	Result = SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, RequiredSize, NULL, &DeviceInfoData);

	if (WiimotesInUse.find(DeviceInfoData.DevInst) != WiimotesInUse.end())
	{
		free(DeviceInterfaceDetailData);
		return;
	}

	HANDLE WiimoteHandle = CheckDevice(DeviceInterfaceDetailData->DevicePath);

	if (WiimoteHandle != INVALID_HANDLE_VALUE)
	{
		NewWiimotes.push_back(WiimoteData(DeviceInfoData.DevInst, WiimoteHandle, IsUsingToshibaStack(&DeviceInfoData)));
	}

	free(DeviceInterfaceDetailData);
}

HANDLE WiimoteScanner::CheckDevice(LPCTSTR DevicePath)
{
	HANDLE OpenDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	if (OpenDevice == INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	HIDD_ATTRIBUTES HidAttributes;
	HidAttributes.Size = sizeof(HidAttributes);

	BOOL Result = HidD_GetAttributes(OpenDevice, &HidAttributes);
	if (!Result)
	{
		CloseHandle(OpenDevice);
		return INVALID_HANDLE_VALUE;
	}

	if ((HidAttributes.VendorID == 0x057e) && ((HidAttributes.ProductID == 0x0306) || (HidAttributes.ProductID == 0x0330)))
	{
		return OpenDevice;
	}
	else
	{
		CloseHandle(OpenDevice);
		return INVALID_HANDLE_VALUE;
	}
}

bool WiimoteScanner::IsUsingToshibaStack(PSP_DEVINFO_DATA DeviceInfoData)
{
	HDEVINFO ParentDeviceInfoSet;
	SP_DEVINFO_DATA ParentDeviceInfoData;
	std::vector<WCHAR> ParentDeviceID;

	bool Result = GetParentDevice(DeviceInfoData->DevInst, ParentDeviceInfoSet, &ParentDeviceInfoData, ParentDeviceID);
	if (!Result)
	{
		return false;
	}

	std::wstring Provider = GetDeviceProperty(ParentDeviceInfoSet, &ParentDeviceInfoData, &DEVPKEY_Device_DriverProvider);

	SetupDiDestroyDeviceInfoList(ParentDeviceInfoSet);

	return (Provider == L"TOSHIBA");
}

bool WiimoteScanner::GetParentDevice(const DEVINST & ChildDevice, HDEVINFO & ParentDeviceInfoSet, PSP_DEVINFO_DATA ParentDeviceInfoData, std::vector<WCHAR> & ParentDeviceID)
{
	ULONG Status;
	ULONG ProblemNumber;
	CONFIGRET Result;

	Result = CM_Get_DevNode_Status(&Status, &ProblemNumber, ChildDevice, 0);
	if (Result != CR_SUCCESS)
	{
		return false;
	}

	DEVINST ParentDevice;

	Result = CM_Get_Parent(&ParentDevice, ChildDevice, 0);
	if (Result != CR_SUCCESS)
	{
		return false;
	}

	ParentDeviceID.resize(MAX_DEVICE_ID_LEN);

	Result = CM_Get_Device_ID(ParentDevice, ParentDeviceID.data(), (ULONG)ParentDeviceID.size(), 0);
	if (Result != CR_SUCCESS)
	{
		return false;
	}

	ParentDeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
	ZeroMemory(ParentDeviceInfoData, sizeof(SP_DEVINFO_DATA));
	ParentDeviceInfoData->cbSize = sizeof(SP_DEVINFO_DATA);

	if (!SetupDiOpenDeviceInfo(ParentDeviceInfoSet, ParentDeviceID.data(), NULL, 0, ParentDeviceInfoData))
	{
		SetupDiDestroyDeviceInfoList(ParentDeviceInfoSet);
		return false;
	}

	return true;
}

std::wstring WiimoteScanner::GetDeviceProperty(HDEVINFO & DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, const DEVPROPKEY * Property)
{
	DWORD RequiredSize = 0;
	DEVPROPTYPE DevicePropertyType;

	SetupDiGetDeviceProperty(DeviceInfoSet, DeviceInfoData, Property, &DevicePropertyType, NULL, 0, &RequiredSize, 0);

	std::vector<BYTE> Buffer(RequiredSize, 0);

	BOOL Result = SetupDiGetDeviceProperty(DeviceInfoSet, DeviceInfoData, Property, &DevicePropertyType, Buffer.data(), RequiredSize, NULL, 0);
	if (!Result)
	{
		return std::wstring();
	}

	return std::wstring((PWCHAR)Buffer.data());
}