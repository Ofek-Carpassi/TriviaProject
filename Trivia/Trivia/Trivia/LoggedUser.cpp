#include "LoggedUser.hpp"
                     
LoggedUser::LoggedUser(std::string username) : m_username(username) { }

LoggedUser::~LoggedUser() { }

std::string LoggedUser::getUsername() const {
    return this->m_username;
}

