#ifndef DATASTORE_H
#define DATASTORE_H

#include <string>
#include <vector>

#include "intakerecord.h"
#include "reminder.h"
#include "user.h"

// ---------------------------------------------------------------
// DataStore  -  File handling with standard C++ (<fstream>)
//
// This is the ONLY class that touches files. It replaces the SQLite
// database of the Qt version. Three plain text files are used, one
// record per line, fields separated by '|' :
//
//   smr_profile.txt    name|age
//   smr_medicines.txt  id|name|dosage|HH:MM|reminder(1/0)
//   smr_history.txt    medicineId|name|dosage|HH:MM|date|recordedAt|TAKEN/MISSED
//
// Safe behaviour:
//   * a missing or empty file simply means "no data yet"
//   * a damaged line is skipped and a warning is remembered
//   * a file that cannot be written throws FileException
// ---------------------------------------------------------------
class DataStore
{
public:
    // filePrefix is put in front of the file names (normally empty).
    explicit DataStore(const std::string &filePrefix = "");

    // ----- profile -----
    bool loadProfile(User &user);                 // false if no profile saved yet
    void saveProfile(const User &user);

    // ----- medicines -----
    std::vector<Reminder> loadMedicines();
    void saveMedicines(const std::vector<Reminder> &medicines);

    // ----- intake history -----
    std::vector<IntakeRecord> loadHistory();
    void saveHistory(const std::vector<IntakeRecord> &records);

    // Problems found while loading (damaged lines etc.), for the user to read.
    const std::vector<std::string> &warnings() const { return warnings_; }
    void clearWarnings() { warnings_.clear(); }

    // Names of the files (shown in the "About" screen).
    std::string profilePath() const { return profilePath_; }
    std::string medicinesPath() const { return medicinesPath_; }
    std::string historyPath() const { return historyPath_; }

private:
    static bool readLines(const std::string &path, std::vector<std::string> &lines);
    static void writeLines(const std::string &path, const std::vector<std::string> &lines);
    void warn(const std::string &file, std::size_t lineNumber, const std::string &reason);

    std::string profilePath_;
    std::string medicinesPath_;
    std::string historyPath_;
    std::vector<std::string> warnings_;
};

#endif // DATASTORE_H
