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
        }

        private void OnNewWiimoteFound(object sender, Wiimote e)
        {
            MessageBox.Show("New Wiimote Found!");
        }
    }
}
