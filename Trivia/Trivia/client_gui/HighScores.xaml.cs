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
            set
            {
                _topScores = value;
                OnPropertyChanged(nameof(TopScores));
            }
        }

        public HighScoresViewModel()
        {
            LoadScores();
        }

        private void LoadScores()
        {
            // Sample data - in a real app you'd fetch from server/database
            var scores = new List<ScoreEntry>
            {
                new ScoreEntry { Username = "Player1", Score = 1000 },
                new ScoreEntry { Username = "Player2", Score = 800 },
                new ScoreEntry { Username = "Player3", Score = 600 },
                new ScoreEntry { Username = "Player4", Score = 500 },
                new ScoreEntry { Username = "Player5", Score = 400 }
            };

            // Sort by score and assign ranks
            TopScores = scores.OrderByDescending(s => s.Score)
                            .Take(10)
                            .Select((s, i) => new ScoreEntry
                            {
                                Username = s.Username,
                                Score = s.Score
                            })
                            .ToList();
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}