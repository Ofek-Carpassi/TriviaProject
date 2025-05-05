#pragma once
#include <string>
#include <regex>

class UserValidator {
public:
    static bool isValidPassword(const std::string& password);
    static bool isValidEmail(const std::string& email);
    static bool isValidStreet(const std::string& street);
    static bool isValidApartment(const std::string& apt);
    static bool isValidCity(const std::string& city);
    static bool isValidPhone(const std::string& phone);
    static bool isValidBirthDate(const std::string& birthDate);
};