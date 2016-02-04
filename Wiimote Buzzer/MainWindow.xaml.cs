using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
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
        List<Color> BuzzerColors = new List<Color> { Colors.Orange, Colors.Green, Colors.Purple, Colors.RoyalBlue };

        List<Buzzer> BuzzedList = new List<Buzzer>();

        public MainWindow()
        {
            InitializeComponent();

            Closing += WindowsClosing;

            WiimoteHub.NewWiimoteFound += OnNewWiimoteFound;
            WiimoteHub.StartScanning();

            BuzzerPanel.ItemsSource = Buzzer;


            int WiimoteIndex = AvailableIndices[0];
            AvailableIndices.RemoveAt(0);
            Buzzer.Add(new Buzzer("Team " + (WiimoteIndex + 1), BuzzerColors[WiimoteIndex], WiimoteIndex, null));
            Buzzer.Sort();
            BuzzerPanel.Items.Refresh();
        }

        private void WindowsClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            WiimoteHub.StopScanning();
            WiimoteHub.DisconnectWiimotes();
        }

        private void RumbleWiimote(Wiimote Wiimote)
        {
            Task.Factory.StartNew(() => { Wiimote.RumbleBriefly(); });
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
            //RumbleWiimote(NewWiimote);

            Buzzer.Add(new Buzzer("Team " + (WiimoteIndex + 1), BuzzerColors[WiimoteIndex], WiimoteIndex, NewWiimote));
            Buzzer.Sort();
            BuzzerPanel.Items.Refresh();
        }
        
        private void OnButtonPressed(object sender, System.EventArgs e)
        {
            Application.Current.Dispatcher.Invoke(new Action(() => { Buzz(sender as Wiimote); }));
        }

        private void Buzz(Wiimote BuzzedWiimote)
        {
            Buzzer Buzzed = GetBuzzer(BuzzedWiimote);

            if(Buzzed == null)
            {
                return;
            }

            if(BuzzedList.Contains(Buzzed))
            {
                return;
            }

            if(BuzzedList.Count == 0)
            {
                // First one
                RumbleWiimote(BuzzedWiimote);

                // Start Reset Timer
                Task.Factory.StartNew(new Action(() => { BuzzedResetTimer(); }));
            }

            BuzzedList.Add(Buzzed);
            Buzzed.BuzzedNumber = BuzzedList.Count;
        }

        private void BuzzedResetTimer()
        {
            Thread.Sleep(5000);
            Application.Current.Dispatcher.Invoke(new Action(() => { ResetBuzzed(); }));
        }

        private void ResetBuzzed()
        {
            foreach(Buzzer Buzzed in BuzzedList)
            {
                Buzzed.Reset();
            }

            BuzzedList.Clear();
        }

        private void OnWiimoteDisconnected(object sender, System.EventArgs e)
        {
            Application.Current.Dispatcher.Invoke(new Action(() => { RemoveWiimote(sender as Wiimote); }));
        }

        private void RemoveWiimote(Wiimote DisconnectedWiimote)
        {
            Buzzer BuzzerToRemove = GetBuzzer(DisconnectedWiimote);

            if(BuzzerToRemove == null)
            {
                return;
            }

            Buzzer.Remove(BuzzerToRemove);
            BuzzerPanel.Items.Refresh();

            AvailableIndices.Add(BuzzerToRemove.Index);
            AvailableIndices.Sort();
        }

        private Buzzer GetBuzzer(Wiimote Wiimote)
        {
            foreach (Buzzer Tmp in Buzzer)
            {
                if (Tmp.Wiimote == Wiimote)
                {
                    return Tmp;
                }
            }

            return null;
        }

        private void Window_StateChanged(object sender, EventArgs e)
        {
            switch(WindowState)
            {
                case WindowState.Maximized:
                    WindowStyle = WindowStyle.None;

                    break;
                case WindowState.Normal:
                    WindowStyle = WindowStyle.SingleBorderWindow;
                    break;
            }
            
        }

        private void Window_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if(e.Key == Key.Escape)
            {
                WindowState = WindowState.Normal;
            }
        }
    }
}
