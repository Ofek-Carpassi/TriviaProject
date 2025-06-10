using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace client_gui
{
    internal class Communicator
    {
        private static Socket? socket_m;
        private static readonly object socketLock = new object();
        private static readonly string SERVER_IP = "127.0.0.1";
        private static readonly int SERVER_PORT = 8826;
        private static bool isInitialized = false;

        // Static constructor: runs once before first usage
        static Communicator()
        {
            InitializeSocket();
        }

        // Initialize or reinitialize the socket connection
        private static void InitializeSocket()
        {
            lock (socketLock)
            {
                try
                {
                    // Close existing socket if it exists
                    if (socket_m != null)
                    {
                        try
                        {
                            if (socket_m.Connected)
                            {
                                socket_m.Shutdown(SocketShutdown.Both);
                            }
                            socket_m.Close();
                        }
                        catch { /* Ignore errors during cleanup */ }
                    }

                    socket_m = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                    // Set reasonable timeouts
                    socket_m.SendTimeout = 15000; // 15 seconds
                    socket_m.ReceiveTimeout = 15000; // 15 seconds

                    // Connect with error handling
                    try
                    {
                        socket_m.Connect(IPAddress.Parse(SERVER_IP), SERVER_PORT);
                        isInitialized = true;
                    }
                    catch (SocketException se)
                    {
                        throw;
                    }
                }
                catch (Exception ex)
                {
                    isInitialized = false;
                    throw new Exception($"Failed to connect to server: {ex.Message}");
                }
            }
        }

        // Sends the given byte array to the connected server.
        public static void Send(byte[] data)
        {
            lock (socketLock)
            {
                try
                {
                    if (!isInitialized || socket_m == null || !socket_m.Connected)
                    {
                        InitializeSocket();
                    }

                    // Check for null again after possible reinitialization
                    if (socket_m != null && socket_m.Connected)
                    {
                        socket_m.Send(data);
                    }
                    else
                    {
                        throw new Exception("Socket is not connected after initialization attempt");
                    }
                }
                catch (Exception ex)
                {
                    isInitialized = false;
                    throw new Exception($"Send error: {ex.Message}", ex);
                }
            }
        }

        // Receives a full Message from the server.
        public static (byte status, string json) Receive()
        {
            lock (socketLock)
            {
                if (!isInitialized)
                    InitializeSocket();

                try
                {
                    // Receive status code
                    byte status = ReceiveExact(1)[0];

                    // Receive message length
                    byte[] lengthBuffer = ReceiveExact(4);
                    int length = (lengthBuffer[0] << 24) | (lengthBuffer[1] << 16) |
                                 (lengthBuffer[2] << 8) | lengthBuffer[3];


                    if (length <= 0 || length > 10485760) // 10MB max for sanity check
                    {
                        throw new Exception($"Invalid message length: {length}");
                    }

                    // Read JSON Message
                    byte[] jsonBuffer = ReceiveExact(length);
                    string finalJson = Encoding.UTF8.GetString(jsonBuffer);



                    // Debug output for JSON content
                    Console.WriteLine($"Received JSON: {(finalJson.Length > 100 ? finalJson.Substring(0, 100) + "..." : finalJson)}");

                    return (status, finalJson);
                }
                catch (Exception ex)
                {
                    isInitialized = false;
                    Console.WriteLine($"Receive error: {ex.GetType().Name} - {ex.Message}");
                    throw new Exception($"Receive error: {ex.Message}", ex);
                }
            }
        }

        // Disconnects and closes the socket.
        public static void Disconnect()
        {
            lock (socketLock)
            {
                if (socket_m != null && socket_m.Connected)
                {
                    try
                    {
                        socket_m.Shutdown(SocketShutdown.Both);
                        socket_m.Close();
                    }
                    catch { /* Ignore errors during cleanup */ }

                    socket_m = null;
                    isInitialized = false;
                }
            }
        }

        // Receives an exact number of bytes from the socket.
        private static byte[] ReceiveExact(int count)
        {
            if (count == 0)
                return new byte[0];

            // Check for null
            if (socket_m == null)
                throw new Exception("Socket is null in ReceiveExact");

            byte[] buffer = new byte[count];
            int received = 0;
            int attempts = 0;
            const int maxAttempts = 5;

            while (received < count && attempts < maxAttempts)
            {
                try
                {
                    int r = socket_m.Receive(buffer, received, count - received, SocketFlags.None);
                    if (r == 0)
                    {
                        attempts++;
                        continue;
                    }
                    received += r;
                }
                catch (SocketException se)
                {
                    if (se.SocketErrorCode == SocketError.TimedOut)
                    {
                        attempts++;
                        continue;
                    }
                    throw;
                }
            }

            if (received < count)
            {
                throw new Exception("Failed to receive complete message after multiple attempts");
            }

            return buffer;
        }
    }
}