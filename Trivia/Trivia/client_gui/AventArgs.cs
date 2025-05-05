using System;

namespace client_gui
{
    /// <summary>
    /// Event arguments for login success
    /// </summary>
    public class LoginSuccessEventArgs : EventArgs
    {
        public string Username { get; set; } = string.Empty;
    }

    /// <summary>
    /// Event arguments for signup success
    /// </summary>
    public class SignupSuccessEventArgs : EventArgs
    {
        public string Username { get; set; } = string.Empty;
    }
}