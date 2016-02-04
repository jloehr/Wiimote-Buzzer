#pragma once

typedef std::set<DWORD> DeviceInstanceIdSet;

class WiimoteScanner
{
public:
	struct WiimoteData {
		DWORD DeviceInstanceId;
		HANDLE DeviceHandle;
		bool UsesToshibaStack;

		WiimoteData(DWORD DeviceInstanceId, HANDLE DeviceHandle, bool UsesToshibaStack)
			:DeviceInstanceId(DeviceInstanceId), DeviceHandle(DeviceHandle), UsesToshibaStack(UsesToshibaStack)
		{}
	};

	typedef std::vector<WiimoteData> WiimoteDataVector;

	WiimoteScanner(DeviceInstanceIdSet & WiimotesInUse);
	WiimoteDataVector ScanForWiimotes();


private:
	DeviceInstanceIdSet & WiimotesInUse;
	WiimoteDataVector NewWiimotes;

	GUID HidGuid;
	HDEVINFO DeviceInfoSet;

	void CheckEnumeratedDeviceInterface(PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
	HANDLE CheckDevice(LPCTSTR DevicePath);

	bool IsUsingToshibaStack(PSP_DEVINFO_DATA DeviceInfoData);
	bool GetParentDevice(const DEVINST & ChildDevice, HDEVINFO & ParentDeviceInfoSet, PSP_DEVINFO_DATA ParentDeviceInfoData, std::vector<WCHAR> & ParentDeviceID);
	std::wstring GetDeviceProperty(HDEVINFO &DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, const DEVPROPKEY * Property);
};

