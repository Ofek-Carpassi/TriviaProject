using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace client_gui
{
    public partial class HighScores : Page
    {
        public HighScores()
        {
            InitializeComponent();
            DataContext = new HighScoresViewModel();
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
    }

    public class HighScoresViewModel : INotifyPropertyChanged
    {
        private List<ScoreEntry> _topScores = new List<ScoreEntry>();
        public List<ScoreEntry> TopScores
        {
            get => _topScores;
            set { _topScores = value; OnPropertyChanged(nameof(TopScores)); }
        }

        public HighScoresViewModel()
        {
            LoadScores();
        }

        private void LoadScores()
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                byte[] request = Serialization.BuildMessage(
                    MessageCodes.HIGH_SCORE_CODE,
                    JsonConvert.SerializeObject(new { }));

                Communicator.Send(request);
                var response = Communicator.Receive();

                if (response.status == MessageCodes.HIGH_SCORE_RESPONSE_CODE)
                {
                    var stats = JsonConvert.DeserializeObject<HighScoreResponse>(response.json);

                    TopScores = stats.Scores
                        .OrderByDescending(s => s.Score)
                        .Take(10)
                        .ToList();
                }
                else
                {
                    MessageBox.Show("Error getting high scores: " + response.json,
                        "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading high scores: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error in LoadScores: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}