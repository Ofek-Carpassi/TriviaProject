using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
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
            // Sample data - replace with actual data source
            LoadScores();
        }

        private void LoadScores()
        {
            // This is where you'd typically fetch from a database or server
            var scores = new List<ScoreEntry>
            {
                new ScoreEntry { Username = "Player1", Score = 1000 },
                new ScoreEntry { Username = "Player2", Score = 800 },
                new ScoreEntry { Username = "Player3", Score = 600 }
            };

            // Sort and take top 3
            TopScores = scores.OrderByDescending(s => s.Score)
                            .Take(3)
                            .Select((s, i) => new ScoreEntry
                            {
                                Username = s.Username,
                                Score = s.Score
                            })
                            .ToList();
        }

        // Fix: Add the nullable annotation to match the interface
        public event PropertyChangedEventHandler? PropertyChanged;
        
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}