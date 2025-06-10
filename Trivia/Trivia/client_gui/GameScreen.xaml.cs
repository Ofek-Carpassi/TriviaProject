using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Threading;
using Newtonsoft.Json;
using System.Windows.Input;

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
                        // Before displaying, make sure answers dictionary is not null
                        if (questionResponse.answers != null)
                        {
                            DisplayQuestion(questionResponse);
                            _currentQuestionIndex++;
                            QuestionNumberText.Text = $"Question {_currentQuestionIndex}/{_totalQuestions}";
                            StartTimer();
                        }
                        else
                        {
                            MessageBox.Show("Error: Question received with no answers", 
                                "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                            ShowGameResults();
                        }
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
                    ShowGameResults();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error getting question: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error in GetNextQuestion: {ex}");
                ShowGameResults();
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
                    Margin = new Thickness(0, 0, 0, 15), // left, top, right, bottom
                    Padding = new Thickness(20, 15, 20, 15),
                    FontSize = 16,
                    Height = 60,
                    HorizontalContentAlignment = HorizontalAlignment.Left,
                    Background = new SolidColorBrush(Color.FromRgb(255, 255, 255)),
                    BorderBrush = new SolidColorBrush(Color.FromRgb(224, 230, 237)),
                    BorderThickness = new Thickness(2, 2, 2, 2), 
                    Foreground = new SolidColorBrush(Color.FromRgb(44, 62, 80)),
                    Cursor = System.Windows.Input.Cursors.Hand 
                };

                // Add rounded corners with modern style
                answerButton.Style = CreateModernAnswerButtonStyle();

                answerButton.Click += AnswerButton_Click;
                _answerButtons[answerButton] = answer.Key;
                AnswersPanel.Children.Add(answerButton);
            }
        }
        
        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            // Show confirmation dialog before leaving game
            var result = MessageBox.Show("Are you sure you want to leave the game?", 
                                        "Leave Game", 
                                        MessageBoxButton.YesNo, 
                                        MessageBoxImage.Question);
            
            if (result == MessageBoxResult.Yes)
            {
                NavigationService?.Navigate(new Menu());
            }
        }

        private Style CreateModernAnswerButtonStyle()
        {
            Style buttonStyle = new Style(typeof(Button));

            // Set default appearance
            buttonStyle.Setters.Add(new Setter(Button.BackgroundProperty, new SolidColorBrush(Color.FromRgb(255, 255, 255))));
            buttonStyle.Setters.Add(new Setter(Button.BorderBrushProperty, new SolidColorBrush(Color.FromRgb(224, 230, 237))));
            buttonStyle.Setters.Add(new Setter(Button.BorderThicknessProperty, new Thickness(2)));
            buttonStyle.Setters.Add(new Setter(Button.ForegroundProperty, new SolidColorBrush(Color.FromRgb(44, 62, 80))));

            // Create control template for rounded corners
            ControlTemplate template = new ControlTemplate(typeof(Button));

            var border = new FrameworkElementFactory(typeof(Border));
            border.SetValue(Border.BackgroundProperty, new TemplateBindingExtension(Button.BackgroundProperty));
            border.SetValue(Border.BorderBrushProperty, new TemplateBindingExtension(Button.BorderBrushProperty));
            border.SetValue(Border.BorderThicknessProperty, new TemplateBindingExtension(Button.BorderThicknessProperty));
            border.SetValue(Border.CornerRadiusProperty, new CornerRadius(8));
            border.SetValue(Border.PaddingProperty, new TemplateBindingExtension(Button.PaddingProperty));

            var contentPresenter = new FrameworkElementFactory(typeof(ContentPresenter));
            contentPresenter.SetValue(ContentPresenter.HorizontalAlignmentProperty, HorizontalAlignment.Left);
            contentPresenter.SetValue(ContentPresenter.VerticalAlignmentProperty, VerticalAlignment.Center);

            border.AppendChild(contentPresenter);
            template.VisualTree = border;

            // Add hover effects
            var hoverTrigger = new Trigger();
            hoverTrigger.Property = Button.IsMouseOverProperty;
            hoverTrigger.Value = true;
            hoverTrigger.Setters.Add(new Setter(Button.BackgroundProperty, new SolidColorBrush(Color.FromRgb(52, 152, 219))));
            hoverTrigger.Setters.Add(new Setter(Button.ForegroundProperty, new SolidColorBrush(Colors.White)));

            template.Triggers.Add(hoverTrigger);
            buttonStyle.Setters.Add(new Setter(Button.TemplateProperty, template));

            return buttonStyle;
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