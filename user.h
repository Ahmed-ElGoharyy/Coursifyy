#pragma once
#include <string>

using namespace std;


class user {
private:
    string username;
    string password; // Hashed password
    string name;
    string email;
    char role; // 'S' for student, 'A' for admin, etc.

public:
    user();
    user(string username, string password, string name, string email = "", char role = 'U');
    virtual ~user();

    // Getters
    string getUsername() const;
    string getPassword() const;
    string getName() const;
    string getEmail() const;
    char getRole() const;

    // Setters - Changed to const reference parameters
    void setUsername(const string& username);
    void setPassword(const string& password);
    void setName(const string& name);
    void setEmail(const string& email);
    void setRole(char role);

    // Authentication
    bool authenticate(const string& password) const;

    // Password utilities
    static string hashPassword(const string& password);
    static bool isValidUsername(const string& username);
    static bool isValidPassword(const string& password);
};