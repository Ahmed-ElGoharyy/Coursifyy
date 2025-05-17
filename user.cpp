#include "stdafx.h"
#include "user.h"
#include <iostream>
#include <regex>
#include <functional> // for hash

using namespace std;


user::user() : username(""), password(""), name(""), email(""), role('U') {
}

user::user(string username, string password, string name, string email, char role)
    : username(username), password(password), name(name), email(email), role(role) {
}

user::~user() {
}

// Getters
string user::getUsername() const {
    return username;
}

string user::getPassword() const {
    return password;
}

string user::getName() const {
    return name;
}

string user::getEmail() const {
    return email;
}

char user::getRole() const {
    return role;
}

// Setters - Updated to use const reference parameters
void user::setUsername(const string& username) {
    this->username = username;
}

void user::setPassword(const string& password) {
    this->password = password;
}

void user::setName(const string& name) {
    this->name = name;
}

void user::setEmail(const string& email) {
    this->email = email;
}

void user::setRole(char role) {
    this->role = role;
}

bool user::authenticate(const string& inputPassword) const {
    // Compare hashed input password with stored password hash
    string hashedInput = hashPassword(inputPassword);
    return hashedInput == password;
}

string user::hashPassword(const string& password) {
    // Simple hash function for demonstration
    // In a real system, use a proper cryptographic hash function
    hash<string> hasher;
    size_t hashValue = hasher(password);
    return to_string(hashValue);
}

bool user::isValidUsername(const string& username) {
    // Username must be 3-20 characters
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }

    // Only allow alphanumeric and underscores
    regex usernamePattern("^[a-zA-Z0-9_]+$");
    return regex_match(username, usernamePattern);
}

bool user::isValidPassword(const string& password) {
    // Password must be at least 6 characters
    if (password.length() < 6) {
        return false;
    }

    // Must contain at least one uppercase letter
    bool hasUppercase = false;
    // Must contain at least one lowercase letter
    bool hasLowercase = false;
    // Must contain at least one digit
    bool hasDigit = false;

    for (char c : password) {
        if (isupper(c)) hasUppercase = true;
        if (islower(c)) hasLowercase = true;
        if (isdigit(c)) hasDigit = true;
    }

    return hasUppercase && hasLowercase && hasDigit;
}