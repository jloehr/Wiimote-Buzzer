#include "stdafx.h"
#include "WiimoteScanner.h"


WiimoteScanner::WiimoteScanner()
{
}

HandleVector WiimoteScanner::ScanForWiimotes()
{	
	FoundWiimotes.clear();

	return FoundWiimotes;
}
