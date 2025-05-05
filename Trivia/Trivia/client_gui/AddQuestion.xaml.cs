using Newtonsoft.Json;
using System;
using System.Windows;
using System.Windows.Controls;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for AddQuestion.xaml
    /// </summary>
    public partial class AddQuestion : Page
    {
        public AddQuestion()
        {
            InitializeComponent();
        }

        private void SubmitQuestion_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Validate inputs
                if (string.IsNullOrWhiteSpace(QuestionTextBox.Text) ||
                    string.IsNullOrWhiteSpace(CorrectAnswerTextBox.Text) ||
                    string.IsNullOrWhiteSpace(WrongAnswer1TextBox.Text) ||
                    string.IsNullOrWhiteSpace(WrongAnswer2TextBox.Text) ||
                    string.IsNullOrWhiteSpace(WrongAnswer3TextBox.Text))
                {
                    MessageBox.Show("Please fill in all fields.", "Missing Information",
                        MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                // Create question data
                var questionData = new
                {
                    question = QuestionTextBox.Text,
                    correct_answer = CorrectAnswerTextBox.Text,
                    wrong_answers = new string[]
                    {
                        WrongAnswer1TextBox.Text,
                        WrongAnswer2TextBox.Text,
                        WrongAnswer3TextBox.Text
                    }
                };

                // Serialize to JSON
                string jsonData = JsonConvert.SerializeObject(questionData);

                // Send to server
                App.CommunicatorMutex.WaitOne();
                try
                {
                    byte[] request = Serialization.BuildMessage(
                        MessageCodes.ADD_QUESTION_CODE,
                        jsonData);

                    Communicator.Send(request);
                    var response = Communicator.Receive();

                    // Process response
                    if(response.status == MessageCodes.ADD_QUESTION_RESPONSE_CODE)
                    {
                        var responseData = JsonConvert.DeserializeObject<Response>(response.json);
                        if (responseData.Status == 1)
                        {
                            MessageBox.Show("Question added successfully!", "Success",
                                MessageBoxButton.OK, MessageBoxImage.Information);
                            NavigationService?.Navigate(new Menu());
                        }
                        else
                        {
                            MessageBox.Show("Failed to add question: " + responseData.message,
                                "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                    }
                    else
                    {
                        MessageBox.Show("Unexpected response from server.",
                            "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
                finally
                {
                    if (App.CommunicatorMutex.WaitOne(0))
                        App.CommunicatorMutex.ReleaseMutex();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error adding question: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error in AddQuestion: {ex}");
            }
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            NavigationService?.Navigate(new Menu());
        }
    }
}