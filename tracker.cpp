#include "tracker.h"

#include <algorithm>

#include "exceptions.h"
#include "utils.h"

namespace
{
    // Used by std::stable_sort: later "recordedAt" first.
    bool newer(const IntakeRecord &a, const IntakeRecord &b)
    {
        return a.recordedAt > b.recordedAt;
    }
}

void Tracker::load()
{
    records_ = store_.loadHistory();
}

bool Tracker::record(const Reminder &medicine, const std::string &date, IntakeStatus status)
{
    if (medicine.id() <= 0)
        throw ValidationException("Please select a saved medicine first.");

    if (!util::isValidDate(date))
        throw ValidationException("Please enter a real date as YYYY-MM-DD (for example 2026-10-17).");

    if (date > util::today())   // ISO dates compare correctly as text
        throw ValidationException("You cannot record a dose for a future date.");

    IntakeRecord fresh;
    fresh.medicineId = medicine.id();
    fresh.medicineName = medicine.name();
    fresh.dosage = medicine.dosage();
    fresh.scheduledTime = medicine.time();
    fresh.doseDate = date;
    fresh.recordedAt = util::nowStamp();
    fresh.status = status;

    std::vector<IntakeRecord> updated = records_;
    bool replaced = false;
    for (std::size_t i = 0; i < updated.size(); ++i)
    {
        if (updated[i].medicineId == fresh.medicineId && updated[i].doseDate == date)
        {
            updated[i] = fresh;
            replaced = true;
            break;
        }
    }
    if (!replaced)
        updated.push_back(fresh);

    store_.saveHistory(updated);   // may throw FileException
    records_ = updated;
    return replaced;
}

bool Tracker::find(int medicineId, const std::string &date, IntakeStatus &status) const
{
    for (std::size_t i = 0; i < records_.size(); ++i)
    {
        if (records_[i].medicineId == medicineId && records_[i].doseDate == date)
        {
            status = records_[i].status;
            return true;
        }
    }
    return false;
}

void Tracker::detachMedicine(int medicineId)
{
    std::vector<IntakeRecord> updated = records_;
    bool changed = false;
    for (std::size_t i = 0; i < updated.size(); ++i)
    {
        if (updated[i].medicineId == medicineId)
        {
            updated[i].medicineId = 0;
            changed = true;
        }
    }

    if (changed)
    {
        store_.saveHistory(updated);
        records_ = updated;
    }
}

void Tracker::clear()
{
    const std::vector<IntakeRecord> empty;
    store_.saveHistory(empty);
    records_ = empty;
}

std::vector<IntakeRecord> Tracker::newestFirst() const
{
    std::vector<IntakeRecord> sorted = records_;
    std::stable_sort(sorted.begin(), sorted.end(), newer);
    return sorted;
}

int Tracker::countStatus(IntakeStatus status) const
{
    int total = 0;
    for (std::size_t i = 0; i < records_.size(); ++i)
    {
        if (records_[i].status == status)
            ++total;
    }
    return total;
}
