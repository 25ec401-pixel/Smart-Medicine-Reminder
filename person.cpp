#include "person.h"

#include <cctype>

#include "exceptions.h"
#include "utils.h"

void Person::setName(const std::string &name)
{
    const std::string trimmed = util::trim(name);

    if (trimmed.empty())
        throw ValidationException("Name cannot be empty.");

    if (trimmed.size() > 60)
        throw ValidationException("Name must be 60 characters or fewer.");

    for (std::size_t i = 0; i < trimmed.size(); ++i)
    {
        const unsigned char c = static_cast<unsigned char>(trimmed[i]);
        const bool allowed = std::isalpha(c) || c >= 128 ||
                             c == ' ' || c == '.' || c == '-' || c == '\'';
        if (!allowed)
            throw ValidationException(
                "Name may contain only letters, spaces, dots, hyphens and apostrophes.");
    }

    name_ = trimmed;
}

void Person::setAge(int age)
{
    if (age < 1 || age > 120)
        throw ValidationException("Age must be between 1 and 120.");

    age_ = age;
}
