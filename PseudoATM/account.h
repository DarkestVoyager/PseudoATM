#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

class Account {
public:
    Account(std::string name, std::string_view pin, std::int64_t sold = 0);
    static Account fromFile(std::string name, std::uint64_t pinHash, std::int64_t sold);

    bool deposit(std::int64_t suma);
    bool withdraw(std::int64_t suma);
    void printInfo() const;

    bool checkPIN(std::string_view pin) const;
    bool checkName(std::string_view name) const;

    std::int64_t getSold() const { return m_sold; }
    const std::string& getName() const { return m_name; }

    static bool isValidPIN(std::string_view pin);
    static std::string setPIN();

    bool save() const;
    static std::optional<Account> load(std::string_view name);
    void printHistory() const;

private:
    static std::uint64_t hashPin(std::string_view pin);
    static std::string toUpper(std::string_view s);
    void logTransaction(std::string_view type, std::int64_t suma) const;
    static std::string fileNameFor(std::string_view name);
    


    std::string m_name{};
    std::int64_t m_sold{};
    std::uint64_t m_pinHash{};
};

#endif // ACCOUNT_H