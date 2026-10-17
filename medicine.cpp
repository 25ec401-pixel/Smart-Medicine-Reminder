#include "medicine.h"

#include "exceptions.h"
#include "utils.h"

Medicine::Medicine(int id, const std::string &name, const std::string &dosage,
                   const std::string &time)
    : id_(id)
{
    setName(name);
    setDosage(dosage);
    setTime(time);
}

void Medicine::setName(const std::string &name)
{
    const std::string trimmed = util::trim(name);

    if (trimmed.empty())
        throw ValidationException("Medicine name cannot be empty.");

    if (trimmed.size() > 60)
        throw ValidationException("Medicine name must be 60 characters or fewer.");

    if (util::hasForbiddenChars(trimmed))
        throw ValidationException("Medicine name cannot contain the '|' character.");

    name_ = trimmed;
}

void Medicine::setDosage(const std::string &dosage)
{
    const std::string trimmed = util::trim(dosage);

    if (trimmed.empty())
        throw ValidationException("Dosage cannot be empty (for example: 500 mg or 1 tablet).");

    if (trimmed.size() > 40)
        throw ValidationException("Dosage must be 40 characters or fewer.");

    if (util::hasForbiddenChars(trimmed))
        throw ValidationException("Dosage cannot contain the '|' character.");

    dosage_ = trimmed;
}

void Medicine::setTime(const std::string &time)
{
    const std::string normal = util::normalizeTime(time);

    if (normal.empty())
        throw ValidationException("Time must look like HH:MM in 24-hour format (for example 08:30 or 20:00).");

    time_ = normal;
}

std::string Medicine::summary() const
{
    return name_ + " (" + dosage_ + ") at " + util::formatTime12(time_);
}
