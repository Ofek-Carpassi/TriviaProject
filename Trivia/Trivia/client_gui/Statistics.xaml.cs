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
    /// Interaction logic for Statistics.xaml
    /// </summary>
    public partial class Statistics : Page
    {
        public Statistics()
        {
            InitializeComponent();
        }
        
        private void PersonalStats_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Navigate to Personal Statistics page
                NavigationService?.Navigate(new PersonalStatics());
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error navigating to personal statistics: {ex.Message}");
                MessageBox.Show("Error opening personal statistics.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Leaderboard_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Navigate to High Scores page
                NavigationService?.Navigate(new HighScores());
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error navigating to leaderboard: {ex.Message}");
                MessageBox.Show("Error opening leaderboard.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                NavigationService?.Navigate(new Menu());
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error navigating back to menu: {ex.Message}");
                MessageBox.Show("Error returning to menu.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}