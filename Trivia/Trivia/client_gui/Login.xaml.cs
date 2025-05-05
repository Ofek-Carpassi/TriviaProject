using System;
using System.Windows;

namespace client_gui
{
    public partial class Login : Window
    {
        public bool LoginSuccess { get; private set; }
        public string? LoginData { get; private set; }

        public Login()
        {
            InitializeComponent();
            var loginChoiceControl = new loginChoice();
            loginChoiceControl.eventOfChoice += SwitchToLogin;
            loginSignup1.Navigate(loginChoiceControl);
        }

        private void SwitchToLogin(object? sender, LoginChoiceArgs args)
        {
            if (args.login == 1)
            {
                var loginInput = new LoginInput();
                loginInput.OnLoginSuccess += (sender, e) =>
                {
                    LoginSuccess = true;
                    LoginData = e.Username;
                    DialogResult = true;
                    Close();
                };
                loginInput.OnLoginFailed += (sender, e) => DialogResult = false;
                loginSignup1.Navigate(loginInput);
            }
            else
            {
                var signupInput = new SignupInput();
                signupInput.OnSignupSuccess += (sender, e) =>
                {
                    LoginSuccess = true;
                    LoginData = e.Username;
                    DialogResult = true;
                    Close();
                };
                signupInput.OnSignupFailed += (sender, e) => DialogResult = false;
                loginSignup1.Navigate(signupInput);
            }
        }
    }
}