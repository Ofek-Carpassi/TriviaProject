using System;
using System.Windows;
using System.Windows.Controls;
using System.Threading;

namespace client_gui
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            try
            {
                Login login = new Login();
                if (login.ShowDialog() == true && login.LoginSuccess)
                {
                    frame1.Navigate(new Menu());
                }
                else
                {
                    Close();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Close();
            }
        }
    }
}