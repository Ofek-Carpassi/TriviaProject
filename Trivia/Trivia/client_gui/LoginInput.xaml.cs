using System;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;

namespace client_gui
{
    public partial class LoginInput : Page
    {
        public event EventHandler<LoginSuccessEventArgs>? OnLoginSuccess;
        public event EventHandler? OnLoginFailed;

        public LoginInput()
        {
            InitializeComponent();
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(UsernameTextBox.Text) ||
                    string.IsNullOrWhiteSpace(PasswordBox.Password))
                {
                    MessageBox.Show("Username and password are required.", "Error",
                        MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                var loginData = new { username = UsernameTextBox.Text, password = PasswordBox.Password };
                string jsonString = JsonConvert.SerializeObject(loginData);

                byte[] msg = Serialization.BuildMessage(MessageCodes.LOGIN_CODE, jsonString);

                try
                {
                    App.CommunicatorMutex.WaitOne();

                    // Send login request without encryption
                    Communicator.Send(msg);

                    // Receive response without decryption
                    var response = Communicator.Receive();
                    Response resp = Deserializer.DeserializeResponse(response);

                    if (resp.Status == MessageCodes.LOGIN_RESPONSE_CODE)
                    {
                        App.m_username = UsernameTextBox.Text;

                        // No more crypto initialization
                        Console.WriteLine($"Login successful for user: {App.m_username}");


                        OnLoginSuccess?.Invoke(this, new LoginSuccessEventArgs { Username = UsernameTextBox.Text });
                    }
                    else
                    {
                        string errorMessage = resp is ErrorResponse error ? error.message : "Invalid username or password.";
                        MessageBox.Show(errorMessage, "Login Failed", MessageBoxButton.OK, MessageBoxImage.Error);
                        OnLoginFailed?.Invoke(this, EventArgs.Empty);
                    }
                }
                finally
                {
                    if (App.CommunicatorMutex.WaitOne(0))
                        App.CommunicatorMutex.ReleaseMutex();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Login error: {ex.GetType().Name} - {ex.Message}");
                MessageBox.Show($"Login error: {ex.Message}", "Error",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                OnLoginFailed?.Invoke(this, EventArgs.Empty);
            }
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            // Navigate back to login choice within the main window
            if (NavigationService != null)
            {
                NavigationService.Navigate(new loginChoice());
            }
            else
            {
                // Fallback: Access MainWindow's frame directly
                if (Application.Current.MainWindow is MainWindow mainWindow)
                {
                    var loginChoiceControl = new loginChoice();
                    // We need to wire up the event handler again
                    loginChoiceControl.eventOfChoice += mainWindow.OnLoginChoice;
                    mainWindow.frame1.Navigate(loginChoiceControl);
                }
            }
        }
    }
}