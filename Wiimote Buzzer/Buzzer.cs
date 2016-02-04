using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;
using WiimoteBuzzerLib;

namespace Wiimote_Buzzer
{
    public class Buzzer : INotifyPropertyChanged, IComparable<Buzzer>
    {
        private string _GroupName;
        private Color _Color;
        private int _BuzzedNumber;

        public event PropertyChangedEventHandler PropertyChanged;

        public Buzzer(string GroupName, Color Color, int Index, Wiimote Wiimote)
        {
            this.GroupName = GroupName;
            this.Color = Color;
            this.Index = Index;
            this.Wiimote = Wiimote;
            this.BuzzedNumber = 0;

        }

        public int Index { get; private set; }
        public Wiimote Wiimote { get; private set;}

        public string GroupName
        {
            get { return _GroupName; }
            set
            {
                _GroupName = value;
                OnPropertyChanged("GroupName");
            }
        }

        public Color Color
        {
            get { return _Color; }
            set
            {
                _Color = value;
                OnPropertyChanged("Color");
            }
        }

        public int BuzzedNumber
        {
            get { return _BuzzedNumber; }
            set
            {
                _BuzzedNumber = value;
                OnPropertyChanged("BuzzedNumber");
                OnPropertyChanged("BuzzedNumberDisplay");
            }
        }

        public string BuzzedNumberDisplay
        {
            get { return (BuzzedNumber == 0) ? "" : BuzzedNumber.ToString(); }
        }

        protected void OnPropertyChanged(string PropertyName)
        {
            PropertyChangedEventHandler Handler = PropertyChanged;
            if (Handler != null)
            {
                Handler(this, new PropertyChangedEventArgs(PropertyName));
            }
        }

        public int CompareTo(Buzzer other)
        {
            return (this.Index - other.Index);
        }

        public void Reset()
        {
            this.BuzzedNumber = 0;
        }

    }
}
