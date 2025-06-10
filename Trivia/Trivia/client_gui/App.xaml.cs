using System;
using System.Threading;
using System.Windows;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public static string m_username = string.Empty;
        public static Mutex CommunicatorMutex = new Mutex();

        // No more encryption-related code needed
    }
}