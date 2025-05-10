#include "stdafx.h"
#include "user.h"
#include <algorithm>
#include <stdexcept>
#include <cctype>

using namespace std;


user::user(string username, string password, string name, string email, char role) {
    setUsername(username);
    setName(name);
    setEmail(email);
    setPassword(password);
    setRole(role);
}


user::user(string username, string password, string name, string email) {
    setUsername(username);
    setName(name);
    setEmail(email);
    setPassword(password);
    setRole('S'); // Default to Student
}


string user::hashPassword(const string& password)
{
    hash<string> hasher;
    size_t h1 = hasher(password);
    size_t h2 = hasher(to_string(h1) + "salt"); // Basic salt
    return to_string(h1 ^ h2); // Combine hashes
}


// Username must be 3-20 alphanumeric chars (underscore allowed)
bool user::isValidUsername(const string& username) {
    if (username.length() < 3 || username.length() > 20) return false;
    return all_of(username.begin(), username.end(), [](char c) {
        return isalnum(c) || c == '_';
        });
}

// Password must be 8+ chars with upper, lower, and digit
bool user::isValidPassword(const string& password) {
    if (password.length() < 8) return false;

    bool hasUpper = false, hasLower = false, hasDigit = false;
    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        else if (islower(c)) hasLower = true;
        else if (isdigit(c)) hasDigit = true;
    }
    return hasUpper && hasLower && hasDigit;
}

// Validate email format using regex
bool user::isValidEmail(const string& email) {
    static const regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(email, pattern);
}

// Validate role (only 'A' or 'S' allowed)
bool user::isValidRole(char role) {
    role = toupper(role);
    return role == 'A' || role == 'S';
}

// Verify if provided password matches stored password
bool user::authenticate(const string& pass) const {
    return hashPassword(pass) == this->password;
}

// Setter methods with validation
void user::setUsername(const string& username) {
    if (!isValidUsername(username)) {
        throw invalid_argument("Invalid username format");
    }
    this->username = username;
}

void user::setName(const string& name) {
    if (name.empty()) {
        throw invalid_argument("Name cannot be empty");
    }
    this->name = name;
}

void user::setEmail(const string& email) {
    if (!isValidEmail(email)) {
        throw invalid_argument("Invalid email format");
    }
    this->email = email;
}

void user::setRole(char role) {
    role = toupper(role);
    if (!isValidRole(role)) {
        throw invalid_argument("Invalid role (must be 'A' or 'S')");
    }
    this->role = role;
}

// Set password after validation
bool user::setPassword(const string& pass) {
    password = pass;
    return true;
}

// Change password after verifying old password
bool user::changePassword(const string& oldPassword, const string& newPassword) {
    if (oldPassword != password) return false;
    if (!isValidPassword(newPassword)) {
        throw invalid_argument("New password doesn't meet requirements");
    }
    password = newPassword;
    return true;
}