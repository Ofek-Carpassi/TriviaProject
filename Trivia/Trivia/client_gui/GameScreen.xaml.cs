using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Threading;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for GameScreen.xaml
    /// </summary>
    public partial class GameScreen : Page
    {
        private int _roomId;
        private int _currentQuestionIndex = 0;
        private int _totalQuestions = 10; // Default value
        private int _timePerQuestion = 20; // Default value in seconds
        private DispatcherTimer _questionTimer;
        private int _timeRemaining;
        private Dictionary<Button, int> _answerButtons = new Dictionary<Button, int>();

        public GameScreen(int roomId)
        {
            InitializeComponent();
            _roomId = roomId;

            // Initialize timer
            _questionTimer = new DispatcherTimer();
            _questionTimer.Interval = TimeSpan.FromSeconds(1);
            _questionTimer.Tick += QuestionTimer_Tick;

            // Start the game
            GetNextQuestion();
        }

        private void GetNextQuestion()
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                // Request next question from server
                byte[] request = Serialization.BuildMessage(
                    MessageCodes.GET_QUESTION_CODE,
                    JsonConvert.SerializeObject(new { }));

                Communicator.Send(request);
                var response = Communicator.Receive();

                if (response.status == MessageCodes.GET_QUESTION_RESPONSE_CODE)
                {
                    var questionResponse = JsonConvert.DeserializeObject<GetQuestionResponse>(response.json);

                    if (questionResponse.status == 1) // Valid question
                    {
                        DisplayQuestion(questionResponse);
                        _currentQuestionIndex++;
                        QuestionNumberText.Text = $"Question {_currentQuestionIndex}/{_totalQuestions}";
                        StartTimer();
                    }
                    else
                    {
                        // No more questions or game is done
                        ShowGameResults();
                    }
                }
                else
                {
                    MessageBox.Show($"Error getting question: {response.json}",
                        "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error getting question: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error in GetNextQuestion: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void DisplayQuestion(GetQuestionResponse question)
        {
            // Display question text
            QuestionText.Text = question.question;

            // Clear previous answers
            AnswersPanel.Children.Clear();
            _answerButtons.Clear();

            // Create buttons for each answer
            foreach (var answer in question.answers)
            {
                Button answerButton = new Button
                {
                    Content = answer.Value,
                    Margin = new Thickness(0, 0, 0, 10),
                    Padding = new Thickness(10),
                    FontSize = 16,
                    Height = 60,
                    HorizontalContentAlignment = HorizontalAlignment.Left
                };

                answerButton.Click += AnswerButton_Click;
                _answerButtons[answerButton] = answer.Key;
                AnswersPanel.Children.Add(answerButton);
            }
        }

        private void StartTimer()
        {
            // Reset the timer
            _timeRemaining = _timePerQuestion;
            TimerText.Text = $"{_timeRemaining}s";
            TimerProgressBar.Value = 100;

            // Start the timer
            _questionTimer.Start();
        }

        private void QuestionTimer_Tick(object sender, EventArgs e)
        {
            _timeRemaining--;
            TimerText.Text = $"{_timeRemaining}s";
            TimerProgressBar.Value = (_timeRemaining / (double)_timePerQuestion) * 100;

            if (_timeRemaining <= 0)
            {
                _questionTimer.Stop();
                TimeUp();
            }
        }

        private void AnswerButton_Click(object sender, RoutedEventArgs e)
        {
            // Stop the timer
            _questionTimer.Stop();

            Button clickedButton = (Button)sender;
            int answerId = _answerButtons[clickedButton];

            // Calculate time taken to answer
            double timeTaken = _timePerQuestion - _timeRemaining;

            // Submit answer to server
            SubmitAnswer(answerId, timeTaken);

            // Get next question
            GetNextQuestion();
        }

        private void TimeUp()
        {
            // If time is up, submit a default answer (most likely wrong or timed out indicator)
            SubmitAnswer(-1, _timePerQuestion);
            GetNextQuestion();
        }

        private void SubmitAnswer(int answerId, double answerTime)
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                // Submit answer to server
                var answerData = new
                {
                    answerId = answerId,
                    answerTime = answerTime
                };

                byte[] request = Serialization.BuildMessage(
                    MessageCodes.SUBMIT_ANSWER_CODE,
                    JsonConvert.SerializeObject(answerData));

                Communicator.Send(request);
                Communicator.Receive(); // We don't need to process this response right now
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error submitting answer: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error in SubmitAnswer: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void ShowGameResults()
        {
            NavigationService?.Navigate(new GameResults(_roomId));
        }
    }

    // Response classes for JSON deserialization
    public class GetQuestionResponse
    {
        public int status { get; set; }
        public string question { get; set; }
        public Dictionary<int, string> answers { get; set; }
    }
}