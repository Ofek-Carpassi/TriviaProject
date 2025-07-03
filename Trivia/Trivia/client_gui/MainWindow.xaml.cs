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

            // Start with login flow directly in the main window
            ShowLoginFlow();
        }

        private void ShowLoginFlow()
        {
            try
            {
                // Create login choice page and navigate to it
                var loginChoiceControl = new loginChoice();
                loginChoiceControl.eventOfChoice += OnLoginChoice;
                frame1.Navigate(loginChoiceControl);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Close();
            }
        }

        public void OnLoginChoice(object? sender, LoginChoiceArgs args)
        {
            try
            {
                if (args.login == 1)
                {
                    // Show login page
                    var loginInput = new LoginInput();
                    loginInput.OnLoginSuccess += OnLoginSuccess;
                    loginInput.OnLoginFailed += OnLoginFailed;
                    frame1.Navigate(loginInput);
                }
                else
                {
                    // Show signup page
                    var signupInput = new SignupInput();
                    signupInput.OnSignupSuccess += OnSignupSuccess;
                    signupInput.OnSignupFailed += OnSignupFailed;
                    frame1.Navigate(signupInput);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void OnLoginSuccess(object? sender, LoginSuccessEventArgs e)
        {
            try
            {
                // Login successful, navigate to menu
                frame1.Navigate(new Menu());
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error navigating to menu: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void OnSignupSuccess(object? sender, SignupSuccessEventArgs e)
        {
            try
            {
                // Signup successful, navigate to menu
                frame1.Navigate(new Menu());
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error navigating to menu: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void OnLoginFailed(object? sender, EventArgs e)
        {
            // Login failed, stay on login page or go back to choice
            // The login page will handle showing the error message
        }

        private void OnSignupFailed(object? sender, EventArgs e)
        {
            // Signup failed, stay on signup page or go back to choice
            // The signup page will handle showing the error message
        }
    }
}