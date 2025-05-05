using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using Newtonsoft.Json;

namespace client_gui.Crypto
{
    /// <summary>
    /// One-Time Pad encryption implementation
    /// </summary>
    public class OTPCryptoAlgorithm : CryptoAlgorithm
    {
        private string _userId;
        private byte[] _key;
        private int _position;
        private static readonly string KeyDirectory = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
            "TriviaGame", "Keys");

        /// <summary>
        /// Creates a new OTP crypto algorithm instance for a specific user
        /// </summary>
        /// <param name="userId">The user ID to use for key storage</param>
        public OTPCryptoAlgorithm(string userId)
        {
            _userId = userId;

            // Ensure key directory exists
            if (!Directory.Exists(KeyDirectory))
            {
                Directory.CreateDirectory(KeyDirectory);
            }

            if (!LoadKeyFromStorage())
            {
                GenerateNewKey();
            }
        }

        /// <summary>
        /// Encrypts a message using the OTP
        /// </summary>
        public override string Encrypt(string message)
        {
            if (_key == null || _key.Length == 0)
            {
                Console.WriteLine("OTP key is empty! Encryption failed.");
                return message;
            }

            // Check if we have enough key material left
            if (_position + message.Length > _key.Length)
            {
                GenerateNewKey(Math.Max(1024, message.Length * 2));
            }

            // Perform XOR operation with key starting at current position
            byte[] messageBytes = Encoding.UTF8.GetBytes(message);
            byte[] result = new byte[messageBytes.Length];

            for (int i = 0; i < messageBytes.Length; i++)
            {
                // Apply XOR with the key at the current position
                int keyIndex = (_position + i) % _key.Length;
                result[i] = (byte)(messageBytes[i] ^ _key[keyIndex]);
            }

            // Update position in the key
            _position += messageBytes.Length;
            SaveKeyToStorage();

            // Return Base64 encoded string
            return Convert.ToBase64String(result);
        }

        /// <summary>
        /// Decrypts a message using the OTP
        /// </summary>
        public override string Decrypt(string message)
        {
            if (_key == null || _key.Length == 0)
            {
                Console.WriteLine("OTP key is empty! Decryption failed.");
                return message;
            }

            try
            {
                // Convert Base64 string back to bytes
                byte[] encryptedBytes = Convert.FromBase64String(message);
                byte[] result = new byte[encryptedBytes.Length];

                // For OTP, encrypt and decrypt operations are identical (XOR is its own inverse)
                for (int i = 0; i < encryptedBytes.Length; i++)
                {
                    int keyIndex = (_position + i) % _key.Length;
                    result[i] = (byte)(encryptedBytes[i] ^ _key[keyIndex]);
                }

                // Update position in the key
                _position += encryptedBytes.Length;
                SaveKeyToStorage();

                return Encoding.UTF8.GetString(result);
            }
            catch (FormatException ex)
            {
                Console.WriteLine($"Decryption error: {ex.Message}");
                return message; // Return original message if decryption fails
            }
        }

        /// <summary>
        /// Generates a new random key
        /// </summary>
        /// <param name="keyLength">The length of the key to generate</param>
        public bool GenerateNewKey(int keyLength = 1024)
        {
            // Generate a random key of specified length
            _key = new byte[keyLength];
            using (var rng = new RNGCryptoServiceProvider())
            {
                rng.GetBytes(_key);
            }

            // Reset position to 0
            _position = 0;

            // Save the new key to storage
            return SaveKeyToStorage();
        }

        /// <summary>
        /// Gets the current position in the key
        /// </summary>
        public int GetKeyPosition()
        {
            return _position;
        }

        /// <summary>
        /// Loads the key from storage
        /// </summary>
        private bool LoadKeyFromStorage()
        {
            try
            {
                string filePath = Path.Combine(KeyDirectory, $"{_userId}.key");
                if (!File.Exists(filePath))
                {
                    return false;
                }

                string json = File.ReadAllText(filePath);
                var keyData = JsonConvert.DeserializeObject<KeyData>(json);

                _key = Convert.FromBase64String(keyData.Key);
                _position = keyData.Position;

                Console.WriteLine($"Loaded OTP key for user {_userId}, position: {_position}");
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error loading OTP key: {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Saves the key to storage
        /// </summary>
        private bool SaveKeyToStorage()
        {
            try
            {
                string filePath = Path.Combine(KeyDirectory, $"{_userId}.key");

                var keyData = new KeyData
                {
                    Key = Convert.ToBase64String(_key),
                    Position = _position
                };

                string json = JsonConvert.SerializeObject(keyData);
                File.WriteAllText(filePath, json);

                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error saving OTP key: {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Key data storage model
        /// </summary>
        private class KeyData
        {
            public string Key { get; set; }
            public int Position { get; set; }
        }
    }
}