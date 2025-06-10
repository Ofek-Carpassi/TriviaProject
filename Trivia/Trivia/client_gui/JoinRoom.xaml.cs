using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : Page
    {
        private Thread? _roomUpdaterThread;
        private bool _isRunning = true;
        private DateTime _lastRefresh = DateTime.MinValue;

        public JoinRoom()
        {
            InitializeComponent();
            Console.WriteLine("JoinRoom page initialized");

            // Clear the example item
            RoomsListBox.Items.Clear();

            // Initial refresh
            RefreshRoomList();

            // Start background updater
            StartRoomUpdaterThread();
        }

        // ADD THIS METHOD - This is what the XAML button is looking for
        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshRoomList();
        }

        private void RefreshRoomList()
        {
            try
            {
                Console.WriteLine("Refreshing room list...");
                _lastRefresh = DateTime.Now;

                // Clear existing rooms
                RoomsListBox.Items.Clear();

                // Show loading indicator
                var loadingItem = new ListBoxItem { Content = "Loading rooms..." };
                RoomsListBox.Items.Add(loadingItem);

                // Send GET_ROOMS request
                byte[] msg = Serialization.BuildMessage(MessageCodes.GET_ROOMS_CODE, "{}");

                try
                {
                    App.CommunicatorMutex.WaitOne();
                    Communicator.Send(msg);
                    var responseData = Communicator.Receive();
                    Console.WriteLine($"Received rooms response with status code: {responseData.status}");

                    var resp = Deserializer.DeserializeResponse(responseData);

                    // Process response
                    if (resp != null && resp.Status == MessageCodes.GET_ROOMS_RESPONSE_CODE)
                    {
                        var roomsResp = resp as GetRoomsResponse;
                        if (roomsResp != null)
                        {
                            UpdateRoomListUI(roomsResp);
                        }
                    }
                    else
                    {
                        Console.WriteLine($"Error getting rooms: {resp?.message ?? "Unknown error"}");
                        // Remove loading indicator and show error
                        RoomsListBox.Items.Clear();
                        RoomsListBox.Items.Add(new ListBoxItem { Content = "Error loading rooms. Try refreshing." });
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error in refresh room list: {ex.Message}");
                    // Remove loading indicator and show error
                    RoomsListBox.Items.Clear();
                    RoomsListBox.Items.Add(new ListBoxItem { Content = $"Connection error: {ex.Message}" });
                }
                finally
                {
                    if (App.CommunicatorMutex.WaitOne(0))
                        App.CommunicatorMutex.ReleaseMutex();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"General error in RefreshRoomList: {ex.Message}");
            }
        }

        private void UpdateRoomListUI(GetRoomsResponse roomsResp)
        {
            try
            {
                RoomsListBox.Items.Clear();

                Console.WriteLine($"Got {roomsResp.Rooms?.Count ?? 0} rooms from server");

                // Debug output of all rooms
                if (roomsResp.Rooms != null)
                {
                    foreach (var room in roomsResp.Rooms)
                    {
                        Console.WriteLine($"Room {room.id}: '{room.name}' - {room.maxPlayers} players, active: {room.isActive}");
                    }
                }

                if (roomsResp.Rooms == null || roomsResp.Rooms.Count == 0)
                {
                    // Show a modern "no rooms" message
                    var noRoomsPanel = new StackPanel
                    {
                        HorizontalAlignment = HorizontalAlignment.Center,
                        VerticalAlignment = VerticalAlignment.Center,
                        Margin = new Thickness(0, 50, 0, 0)
                    };

                    var icon = new TextBlock
                    {
                        Text = "🏠",
                        FontSize = 48,
                        HorizontalAlignment = HorizontalAlignment.Center,
                        Margin = new Thickness(0, 0, 0, 20)
                    };

                    var message = new TextBlock
                    {
                        Text = "No active rooms available",
                        FontSize = 18,
                        FontWeight = FontWeights.Medium,
                        HorizontalAlignment = HorizontalAlignment.Center,
                        Margin = new Thickness(0, 0, 0, 10)
                    };

                    var subMessage = new TextBlock
                    {
                        Text = "Create a new room to start playing!",
                        FontSize = 14,
                        HorizontalAlignment = HorizontalAlignment.Center,
                        Foreground = System.Windows.Media.Brushes.Gray
                    };

                    noRoomsPanel.Children.Add(icon);
                    noRoomsPanel.Children.Add(message);
                    noRoomsPanel.Children.Add(subMessage);

                    var noRoomsItem = new ListBoxItem
                    {
                        Content = noRoomsPanel,
                        IsEnabled = false
                    };
                    RoomsListBox.Items.Add(noRoomsItem);
                }
                else
                {
                    foreach (var room in roomsResp.Rooms)
                    {
                        AddRoomToUI(room);
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error updating room list UI: {ex.Message}");
                MessageBox.Show($"UI update error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void AddRoomToUI(RoomS room)
        {
            try
            {
                // Create a modern room card
                var mainGrid = new Grid
                {
                    Margin = new Thickness(0, 5, 0, 5)
                };

                mainGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star) });
                mainGrid.ColumnDefinitions.Add(new ColumnDefinition { Width = GridLength.Auto });

                // Room info panel
                var infoPanel = new StackPanel
                {
                    VerticalAlignment = VerticalAlignment.Center
                };

                // Get number of players in the room
                int playerCount = GetPlayerCount(room.id);
                string roomStateText = room.isActive ? "🎮 Game In Progress" : "⏳ Waiting for Players";

                var roomNameText = new TextBlock
                {
                    Text = $"🏠 {room.name}",
                    FontSize = 16,
                    FontWeight = FontWeights.Bold,
                    Margin = new Thickness(0, 0, 0, 5),
                    Foreground = (System.Windows.Media.Brush)FindResource("TextPrimaryBrush")
                };

                var roomDetailsText = new TextBlock
                {
                    Text = $"👥 {playerCount}/{room.maxPlayers} players • {roomStateText}",
                    FontSize = 12,
                    Foreground = (System.Windows.Media.Brush)FindResource("TextSecondaryBrush")
                };

                infoPanel.Children.Add(roomNameText);
                infoPanel.Children.Add(roomDetailsText);

                // Join button with modern styling
                int roomId = room.id;
                var joinButton = new Button
                {
                    Content = room.isActive ? "In Progress" : "Join Room",
                    Width = 120,
                    Height = 40, // Increase height slightly
                    IsEnabled = !room.isActive && playerCount < room.maxPlayers,
                    Margin = new Thickness(10, 0, 0, 0),
                    FontSize = 13, // Slightly smaller font
                    VerticalContentAlignment = VerticalAlignment.Center,
                    HorizontalContentAlignment = HorizontalAlignment.Center,
                    Padding = new Thickness(8, 6, 8, 6) // Better padding
                };

                // Apply the modern button style from resources
                if (room.isActive)
                {
                    // Game in progress - use disabled style
                    joinButton.Style = (Style)FindResource("ModernButtonStyle");
                    joinButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(243, 156, 18)); // Orange
                    joinButton.Foreground = System.Windows.Media.Brushes.White;
                }
                else if (playerCount >= room.maxPlayers)
                {
                    // Room full - use danger style
                    joinButton.Content = "Full";
                    joinButton.Style = (Style)FindResource("DangerButtonStyle");
                }
                else
                {
                    // Available room - use primary style
                    joinButton.Style = (Style)FindResource("ModernButtonStyle");
                }

                // Add click handler
                joinButton.Click += (sender, args) =>
                {
                    JoinRoom_Click(roomId);
                };

                Grid.SetColumn(infoPanel, 0);
                Grid.SetColumn(joinButton, 1);

                mainGrid.Children.Add(infoPanel);
                mainGrid.Children.Add(joinButton);

                var item = new ListBoxItem { Content = mainGrid };
                RoomsListBox.Items.Add(item);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error adding room to UI: {ex.Message}");
            }
        }
        private int GetPlayerCount(int roomId)
        {
            try
            {
                App.CommunicatorMutex.WaitOne();
                byte[] getPlayersMsg = Serialization.BuildMessage(
                    MessageCodes.GET_PLAYERS_IN_ROOM_CODE,
                    JsonConvert.SerializeObject(new { roomId }));

                Communicator.Send(getPlayersMsg);
                var playersResponse = Communicator.Receive();
                var playersResp = Deserializer.DeserializeResponse(playersResponse);

                if (playersResp is GetPlayersInRoomResponse playersInRoom)
                {
                    return playersInRoom.Players?.Count ?? 0;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error getting players for room {roomId}: {ex.Message}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }

            return 0;
        }

        private void JoinRoom_Click(int roomId)
        {
            try
            {
                Console.WriteLine($"JOIN BUTTON CLICKED - Joining room with ID: {roomId}");

                var joinData = new { roomId };
                string jsonString = JsonConvert.SerializeObject(joinData);
                Console.WriteLine($"Join data: {jsonString}");
                byte[] msg = Serialization.BuildMessage(MessageCodes.JOIN_ROOM_CODE, jsonString);

                Response resp;
                try
                {
                    App.CommunicatorMutex.WaitOne();
                    Console.WriteLine("Sending join room request...");
                    Communicator.Send(msg);
                    var response = Communicator.Receive();
                    Console.WriteLine($"Join room response received: {response.status}");
                    resp = Deserializer.DeserializeResponse(response);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error joining room: {ex.Message}");
                    MessageBox.Show($"Connection error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }
                finally
                {
                    if (App.CommunicatorMutex.WaitOne(0))
                        App.CommunicatorMutex.ReleaseMutex();
                }

                if (resp.Status == MessageCodes.JOIN_ROOM_RESPONSE_CODE)
                {
                    Console.WriteLine($"Successfully joined room {roomId}");
                    _isRunning = false; // Stop background thread
                    NavigationService?.Navigate(new Room(roomId, false));
                }
                else
                {
                    string error = resp is ErrorResponse errorResp ? errorResp.message : "Join failed.";
                    Console.WriteLine($"Failed to join room: {error}");
                    MessageBox.Show(error, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected error in JoinRoom_Click: {ex.Message}");
                MessageBox.Show($"Error joining room: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void StartRoomUpdaterThread()
        {
            _roomUpdaterThread = new Thread(() =>
            {
                while (_isRunning)
                {
                    try
                    {
                        // Only refresh if it's been more than 5 seconds since last refresh
                        if ((DateTime.Now - _lastRefresh).TotalSeconds > 5)
                        {
                            Dispatcher.Invoke(() => RefreshRoomList());
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"Error in room updater thread: {ex.Message}");
                    }

                    Thread.Sleep(3000); // Check every 3 seconds
                }
            });

            _roomUpdaterThread.IsBackground = true;
            _roomUpdaterThread.Start();
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            // Navigate back to the menu
            NavigationService?.Navigate(new Menu());
        }

        // Stop thread when page is unloaded
        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("JoinRoom page unloaded, stopping thread");
            _isRunning = false;
        }

        ~JoinRoom()
        {
            _isRunning = false; // Ensure thread exits if page is destroyed
        }
    }
}