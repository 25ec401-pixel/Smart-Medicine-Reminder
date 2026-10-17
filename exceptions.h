#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// ---------------------------------------------------------------
// Custom exception classes (Exception Handling)
//
//   std::runtime_error
//        |-- ValidationException   (bad user input)
//        |-- FileException         (a data file cannot be written)
//
// Classes "throw" these; the menu code "catches" them and prints
// a friendly message instead of crashing.
// ---------------------------------------------------------------
class ValidationException : public std::runtime_error
{
public:
    explicit ValidationException(const std::string &message)
        : std::runtime_error(message) {}
};

class FileException : public std::runtime_error
{
public:
    explicit FileException(const std::string &message)
        : std::runtime_error(message) {}
};

#endif // EXCEPTIONS_H
