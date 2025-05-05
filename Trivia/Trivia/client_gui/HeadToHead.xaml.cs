using Newtonsoft.Json;
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Threading;

namespace client_gui
{
    /// <summary>
    /// Interaction logic for HeadToHead.xaml
    /// </summary>
    public partial class HeadToHead : Page
    {
        private bool _isRunning = true;
        private DispatcherTimer _queueCheckTimer;
        private DispatcherTimer _countdownTimer;
        private int _roomId = 0;
        private int _countdownValue = 5;

        public HeadToHead()
        {
            InitializeComponent();
            InitializeQueueCheck();
        }

        private void InitializeQueueCheck()
        {
            // Join the head-to-head queue
            JoinHeadToHeadQueue();

            // Start a timer to check queue status
            _queueCheckTimer = new DispatcherTimer();
            _queueCheckTimer.Interval = TimeSpan.FromSeconds(2);
            _queueCheckTimer.Tick += QueueCheckTimer_Tick;
            _queueCheckTimer.Start();
        }

        private void JoinHeadToHeadQueue()
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                byte[] request = Serialization.BuildMessage(
                    MessageCodes.JOIN_HEAD_TO_HEAD_CODE,
                    JsonConvert.SerializeObject(new { username = App.m_username }));

                Communicator.Send(request);
                var response = Communicator.Receive();

                // Process response - we might get matched immediately
                if (response.status == MessageCodes.JOIN_HEAD_TO_HEAD_RESPONSE_CODE)
                {
                    var responseData = JsonConvert.DeserializeObject<HeadToHeadResponse>(response.json);

                    // Update queue count
                    QueueCountText.Text = $"Players in queue: {responseData.queueSize}";

                    // If we have a match already, start the game
                    if (responseData.hasMatch)
                    {
                        StartCountdown(responseData.roomId);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error joining head-to-head queue: {ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Console.WriteLine($"Error joining head-to-head queue: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void QueueCheckTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                App.CommunicatorMutex.WaitOne();

                byte[] request = Serialization.BuildMessage(
                    MessageCodes.CHECK_HEAD_TO_HEAD_STATUS_CODE,
                    JsonConvert.SerializeObject(new { username = App.m_username }));

                Communicator.Send(request);
                var response = Communicator.Receive();

                if (response.status == MessageCodes.CHECK_HEAD_TO_HEAD_STATUS_RESPONSE_CODE)
                {
                    var responseData = JsonConvert.DeserializeObject<HeadToHeadResponse>(response.json);

                    // Update queue count
                    QueueCountText.Text = $"Players in queue: {responseData.queueSize}";

                    // If we have a match, start the game
                    if (responseData.hasMatch)
                    {
                        _queueCheckTimer.Stop();
                        StartCountdown(responseData.roomId);
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error checking head-to-head status: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void StartCountdown(int roomId)
        {
            _roomId = roomId;
            StatusText.Text = "Opponent found! Starting soon...";
            CountdownBorder.Visibility = Visibility.Visible;
            CancelButton.IsEnabled = false;

            // Start countdown timer
            _countdownValue = 5;
            CountdownText.Text = _countdownValue.ToString();

            _countdownTimer = new DispatcherTimer();
            _countdownTimer.Interval = TimeSpan.FromSeconds(1);
            _countdownTimer.Tick += CountdownTimer_Tick;
            _countdownTimer.Start();
        }

        private void CountdownTimer_Tick(object sender, EventArgs e)
        {
            _countdownValue--;
            CountdownText.Text = _countdownValue.ToString();

            if (_countdownValue <= 0)
            {
                _countdownTimer.Stop();
                StartHeadToHeadGame();
            }
        }

        private void StartHeadToHeadGame()
        {
            // Navigate to game screen with room ID
            NavigationService?.Navigate(new GameScreen(_roomId));
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            LeaveHeadToHeadQueue();
            NavigationService?.Navigate(new Menu());
        }

        private void LeaveHeadToHeadQueue()
        {
            try
            {
                App.CommunicatorMutex.WaitOne();
                byte[] request = Serialization.BuildMessage(
                    MessageCodes.LEAVE_HEAD_TO_HEAD_CODE,
                    JsonConvert.SerializeObject(new { username = App.m_username }));

                Communicator.Send(request);
                // We don't need to process response here
                Communicator.Receive();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error leaving head-to-head queue: {ex}");
            }
            finally
            {
                if (App.CommunicatorMutex.WaitOne(0))
                    App.CommunicatorMutex.ReleaseMutex();
            }
        }

        private void Page_Unloaded(object sender, RoutedEventArgs e)
        {
            _isRunning = false;
            _queueCheckTimer?.Stop();
            _countdownTimer?.Stop();
            LeaveHeadToHeadQueue();
        }
    }

    // Response class for head-to-head matching
    public class HeadToHeadResponse
    {
        public int status { get; set; }
        public int queueSize { get; set; }
        public bool hasMatch { get; set; }
        public int roomId { get; set; }
        public string opponent { get; set; }
    }
}