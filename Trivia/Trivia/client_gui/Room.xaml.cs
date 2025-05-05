using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace client_gui
{
    /// <summary>
    /// Represents a page for a specific trivia game room.
    /// </summary>
    public partial class Room : Page
    {
        private bool _isRunning = true;
        private int _roomId;
        public string admin = string.Empty;
        private bool _isAdmin = false;

        /// <summary>
        /// Initializes a new instance of the Room page for a specific room ID.
        /// </summary>
        /// <param name="roomId">The ID of the room the user has joined.</param>
        public Room(int roomId, bool isAdmin)
        {
            _roomId = roomId;
            _isAdmin = isAdmin;
            InitializeComponent();

            RoomNameText.Text = $"Room: {_roomId}";
            this.Unloaded += Page_Unloaded;

            StartPlayerUpdaterAsync();  // Start the async updater
        }

        /// <summary>
        /// Starts a background task to periodically fetch room state and update the UI.
        /// </summary>
        private async Task StartPlayerUpdaterAsync()
        {
            int consecutiveErrors = 0;

            while (_isRunning)
            {
                try
                {
                    var requestData = new { roomId = _roomId };
                    string jsonString = JsonConvert.SerializeObject(requestData);
                    byte[] msg = Serialization.BuildMessage(MessageCodes.GET_ROOM_STATE_CODE, jsonString);

                    Response resp;
                    App.CommunicatorMutex.WaitOne();
                    try
                    {
                        Communicator.Send(msg);
                        var response = Communicator.Receive();
                        resp = Deserializer.DeserializeResponse(response);
                        consecutiveErrors = 0;
                    }
                    catch (Exception ex)
                    {
                        consecutiveErrors++;

                        if (consecutiveErrors > 5)
                        {
                            _isRunning = false;
                            await Dispatcher.InvokeAsync(() =>
                            {
                                MessageBox.Show("Lost connection to server.", "Connection Error",
                                    MessageBoxButton.OK, MessageBoxImage.Error);
                                NavigationService?.Navigate(new Menu());
                            });
                            break;
                        }

                        await Task.Delay(3000);  // Sleep between retries
                        continue;
                    }
                    finally
                    {
                        App.CommunicatorMutex.ReleaseMutex();
                    }

                    if (resp.Status == MessageCodes.GET_ROOM_STATE_RESPONSE_CODE &&
                        resp is GetRoomStateResponse roomStateResp && roomStateResp.players != null)
                    {
                        await Dispatcher.InvokeAsync(() =>
                        {
                            PlayersListBox.Items.Clear();

                            if (roomStateResp.players.Count > 0)
                            {
                                admin = roomStateResp.players[0];
                                _isAdmin = (admin == App.m_username);
                                AdminNameTextBlock.Text = admin;
                                StartGameButton.Visibility = _isAdmin ? Visibility.Visible : Visibility.Collapsed;

                                foreach (string player in roomStateResp.players)
                                {
                                    PlayersListBox.Items.Add(new ListBoxItem { Content = player });
                                }
                            }
                            else
                            {
                                AdminNameTextBlock.Text = "None";
                                StartGameButton.Visibility = Visibility.Collapsed;
                            }

                            if (roomStateResp.hasGameBegun)
                            {
                                _isRunning = false;
                                MessageBox.Show("Game has started! (Game implementation not completed)",
                                    "Game Started", MessageBoxButton.OK, MessageBoxImage.Information);
                            }
                        });
                    }
                    else if (resp is ErrorResponse errorResp)
                    {
                        if (errorResp.message.Contains("Room not found") ||
                            errorResp.message.Contains("User is not in the room"))
                        {
                            _isRunning = false;
                            await Dispatcher.InvokeAsync(() =>
                            {
                                MessageBox.Show("Room no longer exists or you're not in it.",
                                    "Room Error", MessageBoxButton.OK, MessageBoxImage.Error);
                                NavigationService?.Navigate(new Menu());
                            });
                            break;
                        }
                    }
                }
                catch (Exception ex)
                {
                    await Dispatcher.InvokeAsync(() =>
                    {
                        MessageBox.Show($"Unexpected error: {ex.Message}");
                    });
                }

                await Task.Delay(2000);  // Sleep between room state checks
            }
        }

        /// <summary>
        /// Handles page unload (user navigates away or closes room). Cleans up and stops thread.
        /// </summary>
        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            _isRunning = false;
            LeaveRoomAndStopThread();  // Make sure to stop the task
        }

        /// <summary>
        /// Stops background task, notifies server about leaving the room, and cleans navigation history.
        /// </summary>
        private void LeaveRoomAndStopThread()
        {
            try
            {
                byte code = _isAdmin ? MessageCodes.CLOSE_ROOM_CODE : MessageCodes.LEAVE_ROOM_CODE;
                var requestData = new { roomId = _roomId };
                string jsonString = JsonConvert.SerializeObject(requestData);
                byte[] msg = Serialization.BuildMessage(code, jsonString);

                App.CommunicatorMutex.WaitOne();
                try
                {
                    Communicator.Send(msg);
                    var response = Communicator.Receive(); // Response is ignored
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error sending leave/close message: {ex.Message}");
                }
                finally
                {
                    App.CommunicatorMutex.ReleaseMutex();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error leaving room: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            if (NavigationService != null)
                NavigationService.RemoveBackEntry();
        }

        /// <summary>
        /// Handles the Start Game button click.
        /// </summary>
        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!_isAdmin)
                {
                    MessageBox.Show("Only the room admin can start the game.",
                        "Not Allowed", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                Console.WriteLine("Admin starting the game");
                var requestData = new { roomId = _roomId };
                string jsonString = JsonConvert.SerializeObject(requestData);
                byte[] msg = Serialization.BuildMessage(MessageCodes.START_GAME_CODE, jsonString);

                Response resp;
                App.CommunicatorMutex.WaitOne();
                try
                {
                    Communicator.Send(msg);
                    var response = Communicator.Receive();
                    resp = Deserializer.DeserializeResponse(response);
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Connection error: {ex.Message}",
                        "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }
                finally
                {
                    App.CommunicatorMutex.ReleaseMutex();
                }

                if (resp.Status == MessageCodes.START_GAME_RESPONSE_CODE)
                {
                    MessageBox.Show("Game started successfully!", "Game Started",
                        MessageBoxButton.OK, MessageBoxImage.Information);
                }
                else if (resp is ErrorResponse errorResp)
                {
                    MessageBox.Show($"Failed to start game: {errorResp.message}",
                        "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error starting game: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}
