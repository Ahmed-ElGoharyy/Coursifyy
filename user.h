#pragma once
#include <string>

class user {
private:
    std::string username;
    std::string password; // Hashed password
    std::string name;
    std::string email;
    char role; // 'S' for student, 'A' for admin, etc.

public:
    user();
    user(std::string username, std::string password, std::string name, std::string email = "", char role = 'U');
    virtual ~user();

    // Getters
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getName() const;
    std::string getEmail() const;
    char getRole() const;

    // Setters - Changed to const reference parameters
    void setUsername(const std::string& username);
    void setPassword(const std::string& password);
    void setName(const std::string& name);
    void setEmail(const std::string& email);
    void setRole(char role);

    // Authentication
    bool authenticate(const std::string& password) const;

    // Password utilities
    static std::string hashPassword(const std::string& password);
    static bool isValidUsername(const std::string& username);
    static bool isValidPassword(const std::string& password);
};