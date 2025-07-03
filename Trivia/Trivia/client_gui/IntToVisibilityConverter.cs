using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace client_gui
{
    public class IntToVisibilityConverter : IValueConverter
    {
        // Shows the element if the count is 0 (no scores), otherwise hides it
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is int count)
                return count == 0 ? Visibility.Visible : Visibility.Collapsed;
            return Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}