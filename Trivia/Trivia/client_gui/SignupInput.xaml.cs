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

            // Password validation with better feedback
            if (!ValidatePassword(PasswordBox.Password))
                return false;

            // Email validation
            var emailRegex = new Regex(@"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$");
            if (!emailRegex.IsMatch(EmailBox.Text))
            {
                MessageBox.Show("Invalid email format. Email must be in the format name@domain.com",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // Street validation - letters only
            var streetRegex = new Regex(@"^[a-zA-Z\s]+$");
            if (!streetRegex.IsMatch(StreetBox.Text))
            {
                MessageBox.Show("Street must contain only letters.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // Apartment validation - numbers only
            var aptRegex = new Regex(@"^\d+$");
            if (!aptRegex.IsMatch(AptNumberBox.Text))
            {
                MessageBox.Show("Apartment number must be a number.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // City validation - letters only
            var cityRegex = new Regex(@"^[a-zA-Z\s]+$");
            if (!cityRegex.IsMatch(CityBox.Text))
            {
                MessageBox.Show("City must contain only letters.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // Phone validation - starts with 0, followed by 2-3 digit prefix and 7 digits
            var phoneRegex = new Regex(@"^0(\d{2}|\d{3})\d{7}$");
            if (!phoneRegex.IsMatch(PhoneBox.Text))
            {
                MessageBox.Show("Phone must start with 0 followed by 2-3 digit prefix and 7 digits.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return false;
            }

            // Birth date validation - dd/mm/yyyy format
            var dateRegex = new Regex(@"^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/\d{4}$");
            if (!dateRegex.IsMatch(BirthDateBox.Text))
            {
                MessageBox.Show("Birth date must be in dd/mm/yyyy format.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
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
                    birthDate = BirthDateBox.Text
                };

                string jsonString = JsonConvert.SerializeObject(signupData);
                Console.WriteLine($"Signup data: {jsonString}");

                byte[] msg = Serialization.BuildMessage(MessageCodes.SIGNUP_CODE, jsonString);

                try
                {
                    App.CommunicatorMutex.WaitOne();
                    Console.WriteLine("Sending signup request...");
                    Communicator.Send(msg);
                    Console.WriteLine("Waiting for server response...");
                    var responseData = Communicator.Receive();
                    Console.WriteLine($"Received response: Status={responseData.status}, JSON={responseData.json}");

                    Response resp = Deserializer.DeserializeResponse(responseData);

                    if (resp.Status == MessageCodes.SIGNUP_RESPONSE_CODE)
                    {
                        App.m_username = UsernameBox.Text;

                        // Initialize crypto system with the username
                        App.InitializeCrypto(App.m_username);
                        Console.WriteLine($"Crypto initialized for user: {App.m_username}");

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
                    if (App.CommunicatorMutex.WaitOne(0))
                        App.CommunicatorMutex.ReleaseMutex();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Signup error: {ex.GetType().Name} - {ex.Message}");
                MessageBox.Show($"Signup error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                OnSignupFailed?.Invoke(this, EventArgs.Empty);
            }
        }
    }
}