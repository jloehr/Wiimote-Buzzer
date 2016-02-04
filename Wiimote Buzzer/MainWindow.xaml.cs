using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using WiimoteBuzzerLib;

namespace Wiimote_Buzzer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        WiimoteHub WiimoteHub = new WiimoteHub();
        List<Buzzer> Buzzer = new List<Buzzer>();
        List<int> AvailableIndices = new List<int> { 0, 1, 2, 3 };
        List<Color> BuzzerColors = new List<Color> { Colors.Orange, Colors.Green, Colors.Purple, Colors.Blue };

        public MainWindow()
        {
            InitializeComponent();

            Closing += WindowsClosing;

            WiimoteHub.NewWiimoteFound += OnNewWiimoteFound;
            WiimoteHub.StartScanning();

            BuzzerPanel.ItemsSource = Buzzer;
        }

        private void WindowsClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            WiimoteHub.StopScanning();
            WiimoteHub.DisconnectWiimotes();
        }

        private void OnNewWiimoteFound(object sender, Wiimote e)
        {
            Application.Current.Dispatcher.Invoke(new Action(() => { AddNewWiimote(e); }));       
        }

        private void AddNewWiimote(Wiimote NewWiimote)
        {
            if (AvailableIndices.Count == 0)
            {
                NewWiimote.Disconnect();
            }

            NewWiimote.WiimoteDisconnected += OnWiimoteDisconnected;
            NewWiimote.ButtonPressed += OnButtonPressed;

            int WiimoteIndex = AvailableIndices[0];
            AvailableIndices.RemoveAt(0);

            Wiimote.WiimoteLED LED = Wiimote.WiimoteLED.ALL;

            switch (WiimoteIndex)
            {
                case 0:
                    LED = Wiimote.WiimoteLED.LED1;
                    break;
                case 1:
                    LED = Wiimote.WiimoteLED.LED2;
                    break;
                case 2:
                    LED = Wiimote.WiimoteLED.LED3;
                    break;
                case 3:
                    LED = Wiimote.WiimoteLED.LED4;
                    break;
            }

            NewWiimote.SetLED(LED);
            Task.Factory.StartNew(() => { NewWiimote.RumbleBriefly(); });

            Buzzer.Add(new Buzzer("Team " + (WiimoteIndex + 1), BuzzerColors[WiimoteIndex], WiimoteIndex, NewWiimote));
            Buzzer.Sort();
            BuzzerPanel.Items.Refresh();
        }
        
        private void OnButtonPressed(object sender, System.EventArgs e)
        {
            Wiimote Wiimote = sender as Wiimote;
            Wiimote.RumbleBriefly();
        }

        private void OnWiimoteDisconnected(object sender, System.EventArgs e)
        {
            Application.Current.Dispatcher.Invoke(new Action(() => { RemoveWiimote(sender as Wiimote); }));
        }

        private void RemoveWiimote(Wiimote DisconnectedWiimote)
        {
            Buzzer BuzzerToRemove = null;

            foreach(Buzzer Tmp in Buzzer)
            {
                if(Tmp.Wiimote == DisconnectedWiimote)
                {
                    BuzzerToRemove = Tmp;
                    break;
                }
            }

            if(BuzzerToRemove == null)
            {
                return;
            }

            Buzzer.Remove(BuzzerToRemove);
            BuzzerPanel.Items.Refresh();

            AvailableIndices.Add(BuzzerToRemove.Index);
            AvailableIndices.Sort();
        }
    }
}
