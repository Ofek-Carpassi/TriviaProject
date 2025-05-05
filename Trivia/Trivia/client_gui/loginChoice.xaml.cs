using System;
using System.Windows;
using System.Windows.Controls;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for loginChoice.xaml
    /// </summary>
    public partial class loginChoice : Page
    {
        public event EventHandler<LoginChoiceArgs>? eventOfChoice;

        public loginChoice()
        {
            InitializeComponent();
        }

        private void Login(object sender, RoutedEventArgs e)
        {
            eventOfChoice?.Invoke(this, new LoginChoiceArgs { login = 1 });
        }

        private void Signup(object sender, RoutedEventArgs e)
        {
            eventOfChoice?.Invoke(this, new LoginChoiceArgs { login = 0 });
        }
    }
    public class LoginChoiceArgs
    {
        public int login { get; set; }
    }
}
