#include "medicinemanager.h"

#include <algorithm>

#include "exceptions.h"

namespace
{
    // Used by std::sort: earlier time first, then alphabetical by name.
    bool earlier(const Reminder &a, const Reminder &b)
    {
        if (a.time() != b.time())
            return a.time() < b.time();
        if (a.name() != b.name())
            return a.name() < b.name();
        return a.id() < b.id();
    }
}

void MedicineManager::sortByTime(std::vector<Reminder> &list)
{
    std::sort(list.begin(), list.end(), earlier);
}

void MedicineManager::load()
{
    medicines_ = store_.loadMedicines();
    sortByTime(medicines_);
}

int MedicineManager::nextId() const
{
    int highest = 0;
    for (std::size_t i = 0; i < medicines_.size(); ++i)
        highest = std::max(highest, medicines_[i].id());
    return highest + 1;
}

const Reminder *MedicineManager::find(int id) const
{
    for (std::size_t i = 0; i < medicines_.size(); ++i)
    {
        if (medicines_[i].id() == id)
            return &medicines_[i];
    }
    return nullptr;
}

void MedicineManager::add(const std::string &name, const std::string &dosage,
                          const std::string &time, bool reminderOn)
{
    // The constructor validates every field and throws if one is wrong.
    Reminder fresh(nextId(), name, dosage, time, reminderOn);

    std::vector<Reminder> updated = medicines_;
    updated.push_back(fresh);
    sortByTime(updated);

    store_.saveMedicines(updated);   // may throw FileException
    medicines_ = updated;
}

void MedicineManager::edit(int id, const std::string &name, const std::string &dosage,
                           const std::string &time, bool reminderOn)
{
    if (find(id) == nullptr)
        throw ValidationException("That medicine no longer exists.");

    const Reminder changed(id, name, dosage, time, reminderOn);   // validates

    std::vector<Reminder> updated = medicines_;
    for (std::size_t i = 0; i < updated.size(); ++i)
    {
        if (updated[i].id() == id)
            updated[i] = changed;
    }
    sortByTime(updated);

    store_.saveMedicines(updated);
    medicines_ = updated;
}

void MedicineManager::remove(int id)
{
    if (find(id) == nullptr)
        throw ValidationException("That medicine no longer exists.");

    std::vector<Reminder> updated;
    for (std::size_t i = 0; i < medicines_.size(); ++i)
    {
        if (medicines_[i].id() != id)
            updated.push_back(medicines_[i]);
    }

    store_.saveMedicines(updated);
    medicines_ = updated;
}
