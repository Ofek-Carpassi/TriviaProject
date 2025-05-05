using System;
using System.Threading;
using System.Windows;
using client_gui.Crypto;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        // Add crypto property
        public static CryptoAlgorithm Crypto { get; private set; }

        public static string m_username = string.Empty;
        public static Mutex CommunicatorMutex = new Mutex();

        // Method to initialize crypto once we have username
        public static void InitializeCrypto(string username)
        {
            if (!string.IsNullOrEmpty(username))
            {
                Crypto = new OTPCryptoAlgorithm(username);
                Console.WriteLine($"Crypto initialized for user: {username}");
            }
        }
    }
}