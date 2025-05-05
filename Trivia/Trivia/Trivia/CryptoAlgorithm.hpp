#pragma once
#include <string>

class CryptoAlgorithm
{
public:
    virtual ~CryptoAlgorithm() = default;

    // Pure virtual methods for encryption and decryption
    virtual std::string encrypt(const std::string& message) = 0;
    virtual std::string decrypt(const std::string& message) = 0;
};
