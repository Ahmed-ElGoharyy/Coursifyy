#pragma once
#include <string>
#include <vector>
#include <regex>
#include <functional> // For std::hash

using namespace std;

class user {
private:
    string username;
    string passwordHash;
    string name;
    string email;
    vector<string> roles;

    static string hashPassword(const string& password);
    static bool isValidUsername(const string& username);
    static bool isValidPassword(const string& password);
    static bool isValidEmail(const string& email);

public:
    user() = default;
    user(const string& username, const string& password,
        const string& name, const string& email,
        const vector<string>& roles = {});

    bool authenticate(const string& password) const;

    string getUsername() const { return username; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    const vector<string>& getRoles() const { return roles; }

    void setUsername(const string& username);
    void setName(const string& name);
    void setEmail(const string& email);
    bool changePassword(const string& oldPassword, const string& newPassword);

    void addRole(const string& role);
    void removeRole(const string& role);
};
