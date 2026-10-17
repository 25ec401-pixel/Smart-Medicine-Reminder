#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include <vector>

#include "datastore.h"
#include "intakerecord.h"
#include "reminder.h"

// ---------------------------------------------------------------
// Tracker  -  records and reads medicine intake.
//
// Unlike the very first console version (two int counters that were
// lost on exit), every Taken/Missed action is saved to the history
// file immediately and loaded again at the next start.
// ---------------------------------------------------------------
class Tracker
{
public:
    explicit Tracker(DataStore &store) : store_(store) {}

    void load();

    // Records a dose. Returns true if an older record for the same
    // medicine and date was replaced (so a mistake can be corrected).
    // Throws ValidationException / FileException.
    bool record(const Reminder &medicine, const std::string &date, IntakeStatus status);

    // Looks for a record of this medicine on this date.
    bool find(int medicineId, const std::string &date, IntakeStatus &status) const;

    // When a medicine is deleted its history stays, but loses the link.
    void detachMedicine(int medicineId);

    void clear();

    const std::vector<IntakeRecord> &records() const { return records_; }
    std::vector<IntakeRecord> newestFirst() const;
    int countStatus(IntakeStatus status) const;

private:
    DataStore &store_;                 // not owned
    std::vector<IntakeRecord> records_;
};

#endif // TRACKER_H
