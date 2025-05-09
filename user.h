#pragma once
#include <string>
#include <regex>
#include <functional> // For std::hash

using namespace std;

class user {
private:

    string username;
    string passwordHash;
    string name;
    string email;
    char role;  // 'A' for Admin or 'S' for Student
public:
    static string hashPassword(const string& password);
    static bool isValidUsername(const string& username);
    static bool isValidPassword(const string& password);
    static bool isValidEmail(const string& email);
    static bool isValidRole(char role);

public:
    user() = default;
    user(string username, string password,
        string name, string email, char role);

    bool authenticate(const string& password) const;

    string getUsername() const { return username; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    char getRole() const { return role; }
    string getRoleString() const;  // Returns "Admin" or "Student"
    string getPasswordHash() const { return passwordHash; }

    void setUsername(const string& username);
    void setName(const string& name);
    void setEmail(const string& email);
    void setRole(char role);
    bool setPassword(const string& pass);
    bool changePassword(const string& oldPassword, const string& newPassword);
};