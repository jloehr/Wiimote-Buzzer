#pragma once


namespace WiimoteBuzzerLib {

	public ref class Wiimote
	{
	public:
		Wiimote();
		event System::EventHandler<System::EventArgs^>^ ButtonPressed;
	};
}

