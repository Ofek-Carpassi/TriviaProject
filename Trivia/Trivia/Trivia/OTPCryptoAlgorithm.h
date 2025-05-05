// OTPCryptoAlgorithm.h
#pragma once
#include "CryptoAlgorithm.h"
#include "IDatabase.h"
#include <string>
#include <vector>
#include <random>

class OTPCryptoAlgorithm : public CryptoAlgorithm
{
public:
    // Constructor
    OTPCryptoAlgorithm(IDatabase* database, const std::string& userId);

    // Destructor
    virtual ~OTPCryptoAlgorithm() = default;

    // Encryption and decryption methods
    virtual std::string encrypt(const std::string& message) override;
    virtual std::string decrypt(const std::string& message) override;

    // Generate a new key for a user
    bool generateNewKey(size_t keyLength = 1024);

    // Get the current position in the key
    size_t getKeyPosition() const;

private:
    IDatabase* m_database;
    std::string m_userId;
    std::string m_key;       // The one-time pad key
    size_t m_position;       // Current position in the key

    // Helper methods
    bool loadKeyFromDatabase();
    bool saveKeyToDatabase();
    std::string performXOR(const std::string& message, size_t startPosition);
};