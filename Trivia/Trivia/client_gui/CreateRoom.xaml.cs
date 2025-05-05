using System;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;

namespace client_gui
{
    public partial class CreateRoom : Page
    {
        public CreateRoom()
        {
            InitializeComponent();
        }

        private void CreateRoom_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Validate inputs
                if (string.IsNullOrWhiteSpace(RoomNameTextBox.Text) ||
                    !int.TryParse(TimeTextBox.Text, out int timePerQuestion) ||
                    !int.TryParse(PlayersTextBox.Text, out int maxPlayers) ||
                    !int.TryParse(QuestionCountTextBox.Text, out int questionCount))
                {
                    MessageBox.Show("Please fill all fields with valid values.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                var roomData = new
                {
                    name = RoomNameTextBox.Text, 
                    maxPlayers = maxPlayers,              
                    questionCount = questionCount,  
                    timePerQuestion = timePerQuestion     
                };

                string jsonString = JsonConvert.SerializeObject(roomData);
                Console.WriteLine($"Creating room with data: {jsonString}");
                byte[] msg = Serialization.BuildMessage(MessageCodes.CREATE_ROOM_CODE, jsonString);

                Response resp;
                try
                {
                    // Use mutex to prevent concurrent access to the socket
                    App.CommunicatorMutex.WaitOne();
                    Console.WriteLine("Sending create room request...");
                    Communicator.Send(msg);
                    var responseData = Communicator.Receive();
                    Console.WriteLine($"Received response with code {responseData.status}");
                    resp = Deserializer.DeserializeResponse(responseData);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error in create room communication: {ex.Message}");
                    MessageBox.Show($"Connection error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }
                finally
                {
                    if (App.CommunicatorMutex.WaitOne(0))
                        App.CommunicatorMutex.ReleaseMutex();
                }

                if (resp.Status == MessageCodes.CREATE_ROOM_RESPONSE_CODE)
                {
                    Console.WriteLine($"Room created successfully with ID: {resp.message}");
                    NavigationService?.Navigate(new Room(Convert.ToInt32(resp.message), true));
                }
                else
                {
                    string error = resp is ErrorResponse errorResp ? errorResp.message : "Room creation failed.";
                    Console.WriteLine($"Room creation failed: {error}");
                    MessageBox.Show(error, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected error in CreateRoom_Click: {ex.Message}");
                MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}