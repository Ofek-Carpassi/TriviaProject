using System;

namespace client_gui.Crypto
{
    /// <summary>
    /// Abstract base class for encryption algorithms
    /// </summary>
    public abstract class CryptoAlgorithm
    {
        /// <summary>
        /// Encrypts a message using the algorithm
        /// </summary>
        /// <param name="message">The message to encrypt</param>
        /// <returns>The encrypted message</returns>
        public abstract string Encrypt(string message);

        /// <summary>
        /// Decrypts a message using the algorithm
        /// </summary>
        /// <param name="message">The encrypted message</param>
        /// <returns>The decrypted message</returns>
        public abstract string Decrypt(string message);
    }
}