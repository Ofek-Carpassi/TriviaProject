using System;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;
using System.ComponentModel;

namespace client_gui
{
    public partial class PersonalStatics : Page, INotifyPropertyChanged
    {
        private string _username = App.m_username;
        private int _gamesPlayed;
        private int _correctAnswers;
        private int _wrongAnswers;
        private float _averageAnswerTime;

        public string Username
        {
            get => _username;
            set { _username = value; OnPropertyChanged(nameof(Username)); }
        }
        public int GamesPlayed
        {
            get => _gamesPlayed;
            set { _gamesPlayed = value; OnPropertyChanged(nameof(GamesPlayed)); }
        }
        public int CorrectAnswers
        {
            get => _correctAnswers;
            set { _correctAnswers = value; OnPropertyChanged(nameof(CorrectAnswers)); }
        }
        public int WrongAnswers
        {
            get => _wrongAnswers;
            set { _wrongAnswers = value; OnPropertyChanged(nameof(WrongAnswers)); }
        }
        public float AverageAnswerTime
        {
            get => _averageAnswerTime;
            set { _averageAnswerTime = value; OnPropertyChanged(nameof(AverageAnswerTime)); }
        }

        public PersonalStatics()
        {
            InitializeComponent();
            DataContext = this;
            LoadStatistics();
        }

        private void LoadStatistics()
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                // Request statistics from server
                byte[] request = Serialization.BuildMessage(
                    MessageCodes.HIGH_SCORE_CODE,
                    JsonConvert.SerializeObject(new { username = App.m_username }));

                Communicator.Send(request);
                var response = Communicator.Receive();

                if (response.status == MessageCodes.HIGH_SCORE_RESPONSE_CODE)
                {
                    // Expecting a list of PlayerStatistics in response.json
                    var stats = JsonConvert.DeserializeObject<List<PlayerStatistics>>(response.json);

                    var userStats = stats.Find(s => s.username == App.m_username);
                    if (userStats != null)
                    {
                        Username = userStats.username;
                        GamesPlayed = userStats.gamesPlayed;
                        CorrectAnswers = userStats.correctAnswerCount;
                        WrongAnswers = userStats.wrongAnswerCount;
                        AverageAnswerTime = userStats.averageAnswerTime;
                    }
                }
                else
                {
                    MessageBox.Show("Error getting statistics: " + response.json,
                        "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading statistics: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error in LoadStatistics: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                NavigationService?.Navigate(new Statistics());
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error navigating back to statistics: {ex.Message}");
                MessageBox.Show("Error returning to statistics.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}