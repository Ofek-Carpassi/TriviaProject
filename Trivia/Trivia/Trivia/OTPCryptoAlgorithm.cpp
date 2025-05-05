// OTPCryptoAlgorithm.cpp
#include "OTPCryptoAlgorithm.h"
#include <iostream>
#include <random>
#include <algorithm>

OTPCryptoAlgorithm::OTPCryptoAlgorithm(IDatabase* database, const std::string& userId)
    : m_database(database), m_userId(userId), m_position(0)
{
    if (!loadKeyFromDatabase()) {
        // If key doesn't exist, generate a new one
        generateNewKey();
    }
}

std::string OTPCryptoAlgorithm::encrypt(const std::string& message)
{
    if (m_key.empty()) {
        std::cerr << "OTP key is empty! Encryption failed." << std::endl;
        return message; // Return the original message if encryption fails
    }

    // Check if we have enough key material left
    if (m_position + message.length() > m_key.length()) {
        // Generate a new key if we're running out
        generateNewKey(std::max(static_cast<size_t>(1024), message.length() * 2));
    }

    // Perform XOR operation with key starting at current position
    std::string encrypted = performXOR(message, m_position);

    // Update position in the key
    m_position += message.length();
    saveKeyToDatabase(); // Save the updated position

    return encrypted;
}

std::string OTPCryptoAlgorithm::decrypt(const std::string& message)
{
    if (m_key.empty()) {
        std::cerr << "OTP key is empty! Decryption failed." << std::endl;
        return message; // Return the original message if decryption fails
    }

    // For OTP, encrypt and decrypt operations are identical (XOR is its own inverse)
    // The key position should already be at the right place for the recipient
    std::string decrypted = performXOR(message, m_position);

    // Update position in the key
    m_position += message.length();
    saveKeyToDatabase(); // Save the updated position

    return decrypted;
}

bool OTPCryptoAlgorithm::generateNewKey(size_t keyLength)
{
    // Generate a random key of the specified length
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    m_key.clear();
    m_key.reserve(keyLength);

    for (size_t i = 0; i < keyLength; ++i) {
        m_key.push_back(static_cast<char>(dis(gen)));
    }

    // Reset position to 0
    m_position = 0;

    // Save the new key to the database
    return saveKeyToDatabase();
}

size_t OTPCryptoAlgorithm::getKeyPosition() const
{
    return m_position;
}

bool OTPCryptoAlgorithm::loadKeyFromDatabase()
{
    try {
        // Try to load existing key
        std::string key;
        size_t position;

        if (m_database->getUserCryptoKey(m_userId, key, position)) {
            m_key = key;
            m_position = position;
            return true;
        }

        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading key from database: " << e.what() << std::endl;
        return false;
    }
}

bool OTPCryptoAlgorithm::saveKeyToDatabase()
{
    try {
        // Save the key and current position
        return m_database->saveUserCryptoKey(m_userId, m_key, m_position);
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving key to database: " << e.what() << std::endl;
        return false;
    }
}

std::string OTPCryptoAlgorithm::performXOR(const std::string& message, size_t startPosition)
{
    std::string result;
    result.reserve(message.length());

    for (size_t i = 0; i < message.length(); ++i) {
        // XOR each character with corresponding key byte
        result.push_back(message[i] ^ m_key[startPosition + i]);
    }

    return result;
}