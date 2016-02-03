#pragma once

typedef std::vector<HANDLE> HandleVector;

class WiimoteScanner
{
public:
	WiimoteScanner();
	HandleVector ScanForWiimotes();

private:
	HandleVector FoundWiimotes;
};

