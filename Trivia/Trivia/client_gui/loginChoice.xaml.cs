using System;
using System.Windows;
using System.Windows.Controls;

namespace client_gui
{
    public partial class loginChoice : Page
    {
        public event EventHandler<LoginChoiceArgs>? eventOfChoice;

        public loginChoice()
        {
            InitializeComponent();
        }

        private void Login(object sender, RoutedEventArgs e)
        {
            try
            {
                Console.WriteLine("Login button clicked");
                eventOfChoice?.Invoke(this, new LoginChoiceArgs { login = 1 });
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error in Login click: {ex.Message}");
                MessageBox.Show($"Error opening login form: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Signup(object sender, RoutedEventArgs e)
        {
            try
            {
                Console.WriteLine("Signup button clicked");
                eventOfChoice?.Invoke(this, new LoginChoiceArgs { login = 0 });
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error in Signup click: {ex.Message}");
                MessageBox.Show($"Error opening signup form: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }

    public class LoginChoiceArgs : EventArgs
    {
        public int login { get; set; }
    }
}