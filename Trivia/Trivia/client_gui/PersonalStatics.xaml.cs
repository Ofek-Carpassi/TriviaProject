using System;
using System.Collections.Generic;
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

namespace client_gui
{
    /// <summary>
    /// Interaction logic for PersonalStatics.xaml
    /// </summary>
    public partial class PersonalStatics : Page
    {
        public PersonalStatics()
        {
            InitializeComponent();
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                NavigationService?.Navigate(new Statistics());
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error navigating back to statistics: {ex.Message}");
                MessageBox.Show("Error returning to statistics.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}
