#ifndef REMINDER_H
#define REMINDER_H

#include <string>

#include "medicine.h"

// ---------------------------------------------------------------
// Reminder  -  Inheritance (same idea as your original project)
//
// Reminder IS-A Medicine that also knows:
//   * whether its daily reminder is switched on
//   * whether it is due at a given time of day
//
// Polymorphism: summary() is overridden to mention the reminder.
// ---------------------------------------------------------------
class Reminder : public Medicine
{
public:
    Reminder() : enabled_(true) {}
    Reminder(int id, const std::string &name, const std::string &dosage,
             const std::string &time, bool enabled);

    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }

    // True if the reminder is on and the scheduled time has already
    // arrived today. (currentTime is "HH:MM"; both are 24-hour so a
    // plain text comparison works.)
    bool isDue(const std::string &currentTime) const;

    // Text shown when a reminder appears.
    std::string notificationText() const;

    // Overrides Medicine::summary()
    std::string summary() const;

private:
    bool enabled_;
};

#endif // REMINDER_H
