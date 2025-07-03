using System;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;
using System.Text.RegularExpressions;

namespace client_gui
{
    public partial class SignupInput : Page
    {
        public event EventHandler<SignupSuccessEventArgs>? OnSignupSuccess;
        public event EventHandler? OnSignupFailed;

        public SignupInput()
        {
            InitializeComponent();

            // Set default values for testing
#if DEBUG
            StreetBox.Text = "MagshiStreet";
            AptNumberBox.Text = "123";
            CityBox.Text = "MagshiCity";
            PhoneBox.Text = "0541234567";
            BirthDateBox.Text = "01/01/1990";
#endif
        }

        private bool ValidatePassword(string password)
        {
            bool hasUppercase = password.Any(char.IsUpper);
            bool hasLowercase = password.Any(char.IsLower);
            bool hasDigit = password.Any(char.IsDigit);
            bool hasSpecial = password.Any(c => !char.IsLetterOrDigit(c));
            bool isLongEnough = password.Length >= 8;

            string errorMessage = "";

            if (!isLongEnough)
                errorMessage += "- Password must be at least 8 characters long\n";
            if (!hasUppercase)
                errorMessage += "- Password must contain at least one uppercase letter\n";
            if (!hasLowercase)
                errorMessage += "- Password must contain at least one lowercase letter\n";
            if (!hasDigit)
                errorMessage += "- Password must contain at least one digit\n";
            if (!hasSpecial)
                errorMessage += "- Password must contain at least one special character\n";

            if (!string.IsNullOrEmpty(errorMessage))
            {
                MessageBox.Show($"Password requirements not met:\n{errorMessage}",
                    "Password Validation", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            return true;
        }

        private void SignUp_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!ValidateInputs())
                {
                    return;
                }

                // Disable the button to prevent double-clicking
                ((Button)sender).IsEnabled = false;

                // Get birth date text from TextBox (since we changed from DatePicker to TextBox)
                string birthDateText = BirthDateBox.Text.Trim();

                // Create signup data with all required fields
                var signupData = new
                {
                    username = UsernameBox.Text,
                    password = PasswordBox.Password,
                    email = EmailBox.Text,
                    street = StreetBox.Text,
                    aptNumber = AptNumberBox.Text,
                    city = CityBox.Text,
                    phone = PhoneBox.Text,
                    birthDate = birthDateText
                };

                string jsonString = JsonConvert.SerializeObject(signupData);
                Console.WriteLine($"Signup data: {jsonString}");

                byte[] msg = Serialization.BuildMessage(MessageCodes.SIGNUP_CODE, jsonString);

                try
                {
                    if (!App.CommunicatorMutex.WaitOne(5000)) // 5 second timeout
                    {
                        throw new TimeoutException("Could not acquire communication mutex");
                    }

                    Console.WriteLine("Sending signup request...");
                    Communicator.Send(msg);
                    Console.WriteLine("Waiting for server response...");

                    var responseData = Communicator.Receive();
                    Console.WriteLine($"Received response: Status={responseData.status}, JSON={responseData.json}");

                    Response resp = Deserializer.DeserializeResponse(responseData);

                    if (resp.Status == MessageCodes.SIGNUP_RESPONSE_CODE)
                    {
                        App.m_username = UsernameBox.Text;
                        Console.WriteLine($"Signup successful for user: {App.m_username}");
                        OnSignupSuccess?.Invoke(this, new SignupSuccessEventArgs { Username = UsernameBox.Text });
                    }
                    else
                    {
                        string errorMessage = resp is ErrorResponse error ? error.message : "Signup failed.";
                        Console.WriteLine($"Signup failed: {errorMessage}");
                        MessageBox.Show(errorMessage, "Signup Failed", MessageBoxButton.OK, MessageBoxImage.Error);
                        OnSignupFailed?.Invoke(this, EventArgs.Empty);
                    }
                }
                finally
                {
                    // Always release the mutex and re-enable the button
                    App.CommunicatorMutex.ReleaseMutex();
                    ((Button)sender).IsEnabled = true;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Signup error: {ex.GetType().Name} - {ex.Message}");
                MessageBox.Show($"Signup error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                OnSignupFailed?.Invoke(this, EventArgs.Empty);

                // Re-enable the button on error
                ((Button)sender).IsEnabled = true;
            }
        }

        private bool ValidateInputs()
        {
            // Check for empty fields
            if (string.IsNullOrWhiteSpace(UsernameBox.Text) ||
                string.IsNullOrWhiteSpace(PasswordBox.Password) ||
                string.IsNullOrWhiteSpace(EmailBox.Text) ||
                string.IsNullOrWhiteSpace(StreetBox.Text) ||
                string.IsNullOrWhiteSpace(AptNumberBox.Text) ||
                string.IsNullOrWhiteSpace(CityBox.Text) ||
                string.IsNullOrWhiteSpace(PhoneBox.Text) ||
                string.IsNullOrWhiteSpace(BirthDateBox.Text))
            {
                MessageBox.Show("All fields are required.", "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // Validate birth date format (dd/mm/yyyy or dd-mm-yyyy or dd.mm.yyyy)
            if (!IsValidBirthDate(BirthDateBox.Text))
            {
                MessageBox.Show("Invalid birth date format. Please use dd/mm/yyyy format (e.g., 15/06/1990).",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // Password validation
            if (!ValidatePassword(PasswordBox.Password))
                return false;

            // Email validation
            var emailRegex = new System.Text.RegularExpressions.Regex(@"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$");
            if (!emailRegex.IsMatch(EmailBox.Text))
            {
                MessageBox.Show("Invalid email format. Email must be in the format name@domain.com",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            return true;
        }

        private bool IsValidBirthDate(string dateText)
        {
            if (string.IsNullOrWhiteSpace(dateText))
                return false;

            // Try to parse different date formats
            string[] formats = { "dd/MM/yyyy", "dd-MM-yyyy", "dd.MM.yyyy", "MM/dd/yyyy" };

            foreach (string format in formats)
            {
                if (DateTime.TryParseExact(dateText, format, null, System.Globalization.DateTimeStyles.None, out DateTime result))
                {
                    // Check if the date is reasonable (not in the future, not too old)
                    if (result <= DateTime.Now && result >= DateTime.Now.AddYears(-120))
                    {
                        return true;
                    }
                }
            }

            return false;
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