using System;
using System.Text;

namespace client_gui
{
    public static class Serialization
    {
        /// <summary>
        /// Builds a message to be sent to the server, with optional encryption
        /// </summary>
        /// <param name="code">The message code</param>
        /// <param name="jsonData">The JSON data as a string</param>
        /// <returns>Byte array containing the message</returns>
        public static byte[] BuildMessage(byte code, string jsonData)
        {
            // Debug the JSON being sent
            Console.WriteLine($"Original JSON: {jsonData}");

            string dataToSend = jsonData;

            // Encrypt if we have a crypto algorithm and it's not login/signup
            if (App.Crypto != null && code != MessageCodes.LOGIN_CODE && code != MessageCodes.SIGNUP_CODE)
            {
                try
                {
                    dataToSend = App.Crypto.Encrypt(jsonData);
                    Console.WriteLine($"Encrypted JSON: {dataToSend}");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Encryption failed: {ex.Message}. Using unencrypted data.");
                    dataToSend = jsonData;
                }
            }

            // Convert JSON to bytes
            byte[] jsonBytes = Encoding.UTF8.GetBytes(dataToSend);
            int jsonLength = jsonBytes.Length;

            // Create result buffer: [1 byte code][4 bytes length][json data]
            byte[] result = new byte[1 + 4 + jsonLength];

            // Set message code (first byte)
            result[0] = code;

            // Set length in big-endian format (4 bytes)
            result[1] = (byte)((jsonLength >> 24) & 0xFF);
            result[2] = (byte)((jsonLength >> 16) & 0xFF);
            result[3] = (byte)((jsonLength >> 8) & 0xFF);
            result[4] = (byte)(jsonLength & 0xFF);

            // Copy the JSON data bytes
            Array.Copy(jsonBytes, 0, result, 5, jsonLength);

            return result;
        }
    }
}