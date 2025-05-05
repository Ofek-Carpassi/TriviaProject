using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for GameResults.xaml
    /// </summary>
    public partial class GameResults : Page
    {
        private ObservableCollection<PlayerResultViewModel> _results;
        private int _roomId;

        public GameResults()
        {
            InitializeComponent();
            _results = new ObservableCollection<PlayerResultViewModel>();
            ResultsGrid.ItemsSource = _results;
        }

        public GameResults(int roomId) : this()
        {
            _roomId = roomId;
            LoadGameResults();
        }

        private void LoadGameResults()
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                // Request game results from server
                var requestData = new { roomId = _roomId };
                byte[] request = Serialization.BuildMessage(
                    MessageCodes.GET_GAME_RESULTS_CODE,
                    JsonConvert.SerializeObject(requestData));

                Communicator.Send(request);
                var response = Communicator.Receive();

                if (response.status == MessageCodes.GET_GAME_RESULTS_RESPONSE_CODE)
                {
                    string jsonData = response.json;
                    var resultsResponse = JsonConvert.DeserializeObject<GetGameResultsResponseData>(jsonData);

                    if (resultsResponse.status == 1) // Game has finished
                    {
                        DisplayResults(resultsResponse.results);
                    }
                    else
                    {
                        // Game hasn't finished yet, show message and retry
                        MessageBox.Show("The game hasn't finished yet. Waiting for all players to complete...",
                            "Game In Progress", MessageBoxButton.OK, MessageBoxImage.Information);

                        // Add a retry mechanism with a timer
                        System.Windows.Threading.DispatcherTimer timer = new System.Windows.Threading.DispatcherTimer();
                        timer.Interval = TimeSpan.FromSeconds(5);
                        timer.Tick += (s, e) => {
                            timer.Stop();
                            LoadGameResults();
                        };
                        timer.Start();
                    }
                }
                else
                {
                    MessageBox.Show("Error getting game results: " + response.json,
                        "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to load game results: " + ex.Message,
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error loading game results: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void DisplayResults(List<PlayerResultData> results)
        {
            // Sort results by correct answers (descending), then by average time (ascending)
            var sortedResults = results
                .OrderByDescending(r => r.correctAnswerCount)
                .ThenBy(r => r.averageAnswerTime)
                .ToList();

            _results.Clear();

            // Find highest score to determine winner(s)
            int highestScore = 0;
            if (sortedResults.Count > 0)
            {
                highestScore = sortedResults[0].correctAnswerCount;
            }

            // Add results to the observable collection with rank
            for (int i = 0; i < sortedResults.Count; i++)
            {
                var result = sortedResults[i];
                bool isWinner = result.correctAnswerCount == highestScore;

                _results.Add(new PlayerResultViewModel
                {
                    Rank = i + 1,
                    Username = result.username,
                    CorrectAnswerCount = result.correctAnswerCount,
                    WrongAnswerCount = result.wrongAnswerCount,
                    AverageAnswerTime = result.averageAnswerTime,
                    IsWinner = isWinner
                });
            }

            // Set winner text
            if (_results.Count > 0)
            {
                var winners = _results.Where(r => r.IsWinner).ToList();

                if (winners.Count == 1)
                {
                    WinnerText.Text = $"🏆 Winner: {winners[0].Username} 🏆";
                }
                else if (winners.Count > 1)
                {
                    WinnerText.Text = "🏆 It's a tie! 🏆";
                }
            }
        }

        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            // Navigate back to menu
            NavigationService?.Navigate(new Menu());
        }

        private void PlayAgain_Click(object sender, RoutedEventArgs e)
        {
            // Navigate to join room screen
            NavigationService?.Navigate(new JoinRoom());
        }
    }

    // View model for player results
    public class PlayerResultViewModel
    {
        public int Rank { get; set; }
        public string Username { get; set; }
        public int CorrectAnswerCount { get; set; }
        public int WrongAnswerCount { get; set; }
        public float AverageAnswerTime { get; set; }
        public bool IsWinner { get; set; }
    }

    // Data transfer objects for JSON deserialization
    public class GetGameResultsResponseData
    {
        public int status { get; set; }
        public List<PlayerResultData> results { get; set; } = new List<PlayerResultData>();
    }

    public class PlayerResultData
    {
        public string username { get; set; }
        public int correctAnswerCount { get; set; }
        public int wrongAnswerCount { get; set; }
        public float averageAnswerTime { get; set; }
    }
}