#include "reminder.h"

#include "utils.h"

Reminder::Reminder(int id, const std::string &name, const std::string &dosage,
                   const std::string &time, bool enabled)
    : Medicine(id, name, dosage, time), enabled_(enabled)
{
}

bool Reminder::isDue(const std::string &currentTime) const
{
    return enabled_ && time() <= currentTime;
}

std::string Reminder::notificationText() const
{
    return "It is time to take " + name() + "\n"
           "  Dosage    : " + dosage() + "\n"
           "  Scheduled : " + util::formatTime12(time());
}

std::string Reminder::summary() const
{
    return Medicine::summary() + (enabled_ ? " [reminder on]" : " [reminder off]");
}
