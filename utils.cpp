#include "utils.h"

#include <cctype>
#include <cstdio>
#include <ctime>
#include <iostream>

namespace util
{

// ======================= text helpers =======================

std::string trim(const std::string &text)
{
    std::size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
        ++start;

    std::size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
        --end;

    return text.substr(start, end - start);
}

std::vector<std::string> split(const std::string &text, char separator)
{
    std::vector<std::string> parts;
    std::string current;

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        if (text[i] == separator)
        {
            parts.push_back(current);
            current.clear();
        }
        else
        {
            current += text[i];
        }
    }
    parts.push_back(current);   // the last piece (also works for an empty text)
    return parts;
}

std::string padRight(const std::string &text, std::size_t width)
{
    if (text.size() >= width)
        return text;
    return text + std::string(width - text.size(), ' ');
}

std::string fit(const std::string &text, std::size_t width)
{
    if (text.size() <= width)
        return text;
    if (width <= 2)
        return text.substr(0, width);
    return text.substr(0, width - 2) + "..";
}

std::string toUpper(const std::string &text)
{
    std::string result = text;
    for (std::size_t i = 0; i < result.size(); ++i)
        result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
    return result;
}

bool parseInt(const std::string &text, int &result)
{
    if (text.empty() || text.size() > 9)
        return false;

    int value = 0;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(text[i])))
            return false;
        value = value * 10 + (text[i] - '0');
    }
    result = value;
    return true;
}

bool hasForbiddenChars(const std::string &text)
{
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == '|' || c < 32 || c == 127)
            return true;
    }
    return false;
}

// ======================= time and date =======================

std::string normalizeTime(const std::string &input)
{
    const std::string text = trim(input);
    const std::size_t colon = text.find(':');
    if (colon == std::string::npos)
        return "";

    const std::string hourPart = text.substr(0, colon);
    const std::string minutePart = text.substr(colon + 1);

    if (hourPart.empty() || hourPart.size() > 2 || minutePart.size() != 2)
        return "";

    int hour = 0;
    int minute = 0;
    if (!parseInt(hourPart, hour) || !parseInt(minutePart, minute))
        return "";
    if (hour > 23 || minute > 59)
        return "";

    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", hour, minute);
    return buffer;
}

bool isValidTime(const std::string &text)
{
    return !normalizeTime(text).empty();
}

bool isValidDate(const std::string &text)
{
    if (text.size() != 10 || text[4] != '-' || text[7] != '-')
        return false;

    int year = 0;
    int month = 0;
    int day = 0;
    if (!parseInt(text.substr(0, 4), year) ||
        !parseInt(text.substr(5, 2), month) ||
        !parseInt(text.substr(8, 2), day))
        return false;

    if (year < 1900 || year > 2200 || month < 1 || month > 12 || day < 1)
        return false;

    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    const bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && leapYear)
        maxDay = 29;

    return day <= maxDay;
}

std::string formatTime12(const std::string &hhmm)
{
    const std::string normal = normalizeTime(hhmm);
    if (normal.empty())
        return hhmm;

    int hour = 0;
    int minute = 0;
    parseInt(normal.substr(0, 2), hour);
    parseInt(normal.substr(3, 2), minute);

    const char *suffix = (hour >= 12) ? "PM" : "AM";
    int shownHour = hour % 12;
    if (shownHour == 0)
        shownHour = 12;

    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d %s", shownHour, minute, suffix);
    return buffer;
}

namespace
{
    // Reads the computer clock and formats it with strftime.
    std::string clockText(const char *format, const char *fallback)
    {
        const std::time_t now = std::time(nullptr);
        const std::tm *local = std::localtime(&now);
        if (local == nullptr)
            return fallback;

        char buffer[32];
        if (std::strftime(buffer, sizeof(buffer), format, local) == 0)
            return fallback;
        return buffer;
    }
}

std::string today()
{
    return clockText("%Y-%m-%d", "1970-01-01");
}

std::string currentTime()
{
    return clockText("%H:%M", "00:00");
}

std::string nowStamp()
{
    return clockText("%Y-%m-%d %H:%M", "1970-01-01 00:00");
}

// ======================= keyboard input =======================

std::string readLine(const std::string &prompt)
{
    std::cout << prompt << std::flush;

    std::string line;
    if (!std::getline(std::cin, line))
        throw InputClosed();   // keyboard closed or input file finished

    return trim(line);
}

int readInt(const std::string &prompt, int minValue, int maxValue)
{
    while (true)
    {
        const std::string line = readLine(prompt);

        int value = 0;
        if (parseInt(line, value) && value >= minValue && value <= maxValue)
            return value;

        std::cout << "  Please enter a whole number from " << minValue
                  << " to " << maxValue << ".\n";
    }
}

bool readYesNo(const std::string &prompt)
{
    while (true)
    {
        const std::string answer = toUpper(readLine(prompt));

        if (answer == "Y" || answer == "YES")
            return true;
        if (answer == "N" || answer == "NO")
            return false;

        std::cout << "  Please type Y or N.\n";
    }
}

void pause()
{
    readLine("\nPress Enter to continue...");
}

// ======================= output =======================

std::string percentText(double value)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f%%", value);
    return buffer;
}

std::string barText(double percent, int width)
{
    if (percent < 0.0)
        percent = 0.0;
    if (percent > 100.0)
        percent = 100.0;

    const int filled = static_cast<int>(percent / 100.0 * width + 0.5);
    return "[" + std::string(static_cast<std::size_t>(filled), '#') +
           std::string(static_cast<std::size_t>(width - filled), '-') + "]";
}

void printLine(char symbol, int count)
{
    std::cout << std::string(static_cast<std::size_t>(count), symbol) << "\n";
}

} // namespace util
