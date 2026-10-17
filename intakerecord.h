#ifndef INTAKERECORD_H
#define INTAKERECORD_H

#include <string>

// Whether a dose was taken or missed.
enum IntakeStatus
{
    STATUS_TAKEN,
    STATUS_MISSED
};

// One line of intake history: "this dose, on this date, was taken/missed".
struct IntakeRecord
{
    int medicineId;               // 0 means the medicine was deleted later
    std::string medicineName;     // saved as a copy so old history stays readable
    std::string dosage;
    std::string scheduledTime;    // "HH:MM"
    std::string doseDate;         // "YYYY-MM-DD"
    std::string recordedAt;       // "YYYY-MM-DD HH:MM"
    IntakeStatus status;

    IntakeRecord() : medicineId(0), status(STATUS_TAKEN) {}
};

inline std::string statusToText(IntakeStatus status)
{
    return status == STATUS_TAKEN ? "Taken" : "Missed";
}

#endif // INTAKERECORD_H
