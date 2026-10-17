#ifndef MEDICINEMANAGER_H
#define MEDICINEMANAGER_H

#include <string>
#include <vector>

#include "datastore.h"
#include "reminder.h"

// ---------------------------------------------------------------
// MedicineManager  -  keeps the list of medicines and saves every
// change to the file straight away (add / edit / delete / find).
//
// Each change is made on a COPY of the list first and only kept if
// saving succeeded, so the list in memory never disagrees with the file.
// ---------------------------------------------------------------
class MedicineManager
{
public:
    explicit MedicineManager(DataStore &store) : store_(store) {}

    void load();                                   // read the file

    // Throw ValidationException (bad input) or FileException (cannot save).
    void add(const std::string &name, const std::string &dosage,
             const std::string &time, bool reminderOn);
    void edit(int id, const std::string &name, const std::string &dosage,
              const std::string &time, bool reminderOn);
    void remove(int id);

    const Reminder *find(int id) const;            // nullptr if not found
    const std::vector<Reminder> &all() const { return medicines_; }   // sorted by time
    std::size_t count() const { return medicines_.size(); }

private:
    static void sortByTime(std::vector<Reminder> &list);
    int nextId() const;

    DataStore &store_;                             // not owned
    std::vector<Reminder> medicines_;
};

#endif // MEDICINEMANAGER_H
