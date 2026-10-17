#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// ---------------------------------------------------------------
// Small helper functions used by every other file.
// Everything is standard C++ (no external libraries).
// ---------------------------------------------------------------

// Thrown when the keyboard input ends (for example Ctrl+Z on Windows).
// main() catches it and closes the program politely.
struct InputClosed
{
};

namespace util
{
    // ----- text helpers -----
    std::string trim(const std::string &text);
    std::vector<std::string> split(const std::string &text, char separator);
    std::string padRight(const std::string &text, std::size_t width);
    std::string fit(const std::string &text, std::size_t width);   // cut long text with ".."
    std::string toUpper(const std::string &text);
    bool parseInt(const std::string &text, int &result);           // digits only, max 9 digits
    bool hasForbiddenChars(const std::string &text);               // '|' or control characters

    // ----- time and date (24-hour "HH:MM", date "YYYY-MM-DD") -----
    std::string normalizeTime(const std::string &text);            // "8:5" -> "" ; "8:05" -> "08:05"
    bool isValidTime(const std::string &text);
    bool isValidDate(const std::string &text);
    std::string formatTime12(const std::string &hhmm);             // "14:30" -> "02:30 PM"
    std::string today();                                           // "2026-10-17"
    std::string currentTime();                                     // "13:45"
    std::string nowStamp();                                        // "2026-10-17 13:45"

    // ----- keyboard input (all of them keep asking until the answer is valid) -----
    std::string readLine(const std::string &prompt);               // may return ""
    int readInt(const std::string &prompt, int minValue, int maxValue);
    bool readYesNo(const std::string &prompt);
    void pause();                                                  // "Press Enter to continue"

    // ----- output -----
    std::string percentText(double value);                         // 83.333 -> "83.33%"
    std::string barText(double percent, int width);                // [########----]
    void printLine(char symbol, int count);
}

#endif // UTILS_H
