#include "UserValidator.h"
#include <regex>

bool UserValidator::isValidPassword(const std::string& password) {
    // Password must be at least 8 characters and contain uppercase, lowercase, digit, and special character
    std::regex passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>/?]).{8,}$");
    return std::regex_match(password, passwordRegex);
}

bool UserValidator::isValidEmail(const std::string& email) {
    // Email must have format <start>@<domain>
    std::regex emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return std::regex_match(email, emailRegex);
}

bool UserValidator::isValidStreet(const std::string& street) {
    // Street must contain only uppercase and lowercase letters
    std::regex streetRegex("^[a-zA-Z\\s]+$");
    return std::regex_match(street, streetRegex);
}

bool UserValidator::isValidApartment(const std::string& apt) {
    // Apartment must be a number
    std::regex aptRegex("^\\d+$");
    return std::regex_match(apt, aptRegex);
}

bool UserValidator::isValidCity(const std::string& city) {
    // City must contain only uppercase and lowercase letters
    std::regex cityRegex("^[a-zA-Z\\s]+$");
    return std::regex_match(city, cityRegex);
}

bool UserValidator::isValidPhone(const std::string& phone) {
    // Phone number - prefix must start with zero and be 2 or 3 digits, followed by number
    std::regex phoneRegex("^0(\\d{2}|\\d{3})\\d{7}$");
    return std::regex_match(phone, phoneRegex);
}

bool UserValidator::isValidBirthDate(const std::string& birthDate) {
    // Birth date must be in dd/mm/yyyy format
    std::regex dateRegex("^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/\\d{4}$");
    return std::regex_match(birthDate, dateRegex);
}