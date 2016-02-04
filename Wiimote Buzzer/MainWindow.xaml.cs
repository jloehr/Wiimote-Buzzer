using System.Windows;
using WiimoteBuzzerLib;

namespace Wiimote_Buzzer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        WiimoteHub WiimoteHub = new WiimoteHub();

        public MainWindow()
        {
            InitializeComponent();

            Closing += WindowsClosing;

            WiimoteHub.NewWiimoteFound += OnNewWiimoteFound;
            WiimoteHub.StartScanning();
        }

        private void WindowsClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            WiimoteHub.StopScanning();
            WiimoteHub.DisconnectWiimotes();
        }

        private void OnNewWiimoteFound(object sender, Wiimote e)
        {
            e.WiimoteDisconnected += OnWiimoteDisconnected;
            e.ButtonPressed += OnButtonPressed;
            e.SetLED(Wiimote.WiimoteLED.LED1);
            e.RumbleBriefly();
        }

        private void OnButtonPressed(object sender, System.EventArgs e)
        {
            Wiimote Wiimote = sender as Wiimote;
            Wiimote.RumbleBriefly();
        }

        private void OnWiimoteDisconnected(object sender, System.EventArgs e)
        {
        }
    }
}
