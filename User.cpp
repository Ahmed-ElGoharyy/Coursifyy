#include "stdafx.h"
#include "user.h"
#include <algorithm>
#include <stdexcept>

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

// Initialize user with validated fields
user::user(const string& username, const string& password,
    const string& name, const string& email,
    const vector<string>& roles) {
    setUsername(username);
    setName(name);
    setEmail(email);

    if (!isValidPassword(password)) {
        throw invalid_argument("Password does not meet requirements");
    }
    passwordHash = hashPassword(password);
    this->roles = roles;
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

// Change password after verifying old password
bool user::changePassword(const string& oldPassword, const string& newPassword) {
    if (hashPassword(oldPassword) != passwordHash) return false;
    if (!isValidPassword(newPassword)) {
        throw invalid_argument("New password doesn't meet requirements");
    }
    passwordHash = hashPassword(newPassword);
    return true;
}

// Role management
void user::addRole(const string& role) {
    if (role.empty()) throw invalid_argument("Role cannot be empty");
    if (find(roles.begin(), roles.end(), role) == roles.end()) {
        roles.push_back(role);
    }
}

void user::removeRole(const string& role) {
    roles.erase(remove(roles.begin(), roles.end(), role), roles.end());
}
