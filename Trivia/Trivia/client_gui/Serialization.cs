using System;
using System.Text;

namespace client_gui
{
    public static class Serialization
    {
        /// <summary>
        /// Builds a message to be sent to the server (no encryption)
        /// </summary>
        /// <param name="code">The message code</param>
        /// <param name="jsonData">The JSON data as a string</param>
        /// <returns>Byte array containing the message</returns>
        public static byte[] BuildMessage(byte code, string jsonData)
        {
            // Debug the JSON being sent
            Console.WriteLine($"Sending JSON: {jsonData}");

            // Convert JSON to bytes
            byte[] jsonBytes = Encoding.UTF8.GetBytes(jsonData);
            int jsonLength = jsonBytes.Length;

            // Create result buffer: [1 byte code][4 bytes length][json data]
            byte[] result = new byte[1 + 4 + jsonLength];

            // Set message code (first byte)
            result[0] = code;

            // Set message length (next 4 bytes, big endian)
            result[1] = (byte)((jsonLength >> 24) & 0xFF);
            result[2] = (byte)((jsonLength >> 16) & 0xFF);
            result[3] = (byte)((jsonLength >> 8) & 0xFF);
            result[4] = (byte)(jsonLength & 0xFF);

            // Copy JSON data to result buffer
            Buffer.BlockCopy(jsonBytes, 0, result, 5, jsonLength);

            return result;
        }
    }
}