using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Wiimote_Buzzer
{
    /// <summary>
    /// Interaction logic for BuzzerTemplate.xaml
    /// </summary>
    public partial class BuzzerTemplate : UserControl
    {
        public BuzzerTemplate()
        {
            InitializeComponent();
        }

        private void GroupName_MouseDown(object sender, MouseButtonEventArgs e)
        {
            GroupName.Visibility = Visibility.Hidden;
            GroupNameEditor.Visibility = Visibility.Visible;
            e.Handled = true;
        }

        private void Grid_MouseDown(object sender, MouseButtonEventArgs e)
        {
            GroupNameEditor.Visibility = Visibility.Hidden;
            GroupName.Visibility = Visibility.Visible;

        }
    }

    public class ColorConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            Color OldColor = (Color)values[0];
            int Number = System.Convert.ToInt32(values[1]);
            Number = (Number == 0) ? 5 : Number;

            float ColorStates = 6f;
            float ColorMulti = (ColorStates - (float)Number) / ColorStates;

            OldColor.ScA = ColorMulti;

            return OldColor;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
