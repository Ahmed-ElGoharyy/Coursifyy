#include "stdafx.h"
#include "user.h"
#include <algorithm>
#include <stdexcept>
#include <cctype>

using namespace std;

// Create password hash with simple salting
string user::hashPassword(const string& password) {
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

// Initialize user with validated fields
user::user(string username, string password, string name, string email, char role) {
    setUsername(username);
    setName(name);
    setEmail(email);
    setRole(role);

    if (!isValidPassword(password)) {
        throw invalid_argument("Password does not meet requirements");
    }
    passwordHash = hashPassword(password);
}

// Returns role as string ("Admin" or "Student")
string user::getRoleString() const {
    switch (role) {
    case 'A': return "Admin";
    case 'S': return "Student";
    default: return "Unknown";
    }
}

// Verify if provided password matches stored hash
bool user::authenticate(const string& password) const {
    return hashPassword(password) == passwordHash;
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
        throw invalid_argument("Role must be 'A' (Admin) or 'S' (Student)");
    }
    this->role = role;
}

// Change password after verifying old password
bool user::changePassword(const string& oldPassword, const string& newPassword) {
    if (hashPassword(oldPassword) != passwordHash) return false;
    if (!isValidPassword(newPassword)) {
        throw invalid_argument("New password doesn't meet requirements");
    }
    passwordHash = hashPassword(newPassword);
    return true;
}
bool user::setPassword(const string& pass) {
    passwordHash = hashPassword(pass);
    return true;
}