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
    public partial class Menu : Page
    {
        public Menu()
        {
            InitializeComponent();
        }

        private void CreateRoom_Click(object sender, RoutedEventArgs e)
        {
            // Navigate to CreateRoom page
            if (NavigationService != null)
            {
                NavigationService.Navigate(new CreateRoom());
            }
            else
            {
                // Fallback: Access MainWindow's frame
                if (Application.Current.MainWindow is MainWindow mainWindow)
                {
                    mainWindow.frame1.Navigate(new CreateRoom());
                }
            }
        }

        private void Statistics_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                NavigationService?.Navigate(new Statistics());
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error navigating to statistics: {ex.Message}");
                MessageBox.Show("Error opening statistics page.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void JoinRoom_Click(object sender, RoutedEventArgs e)
        {
            // Navigate to JoinRoom page
            if (NavigationService != null)
            {
                NavigationService.Navigate(new JoinRoom());
            }
            else
            {
                // Fallback: Access MainWindow's frame
                if (Application.Current.MainWindow is MainWindow mainWindow)
                {
                    mainWindow.frame1.Navigate(new JoinRoom());
                }
            }
        }

        private void Exit_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void AddQuestion_Click(object sender, RoutedEventArgs e)
        {
            NavigationService?.Navigate(new AddQuestion());
        }

        private void HeadToHead_Click(object sender, RoutedEventArgs e)
        {
            NavigationService?.Navigate(new HeadToHead());
        }
    }
}
