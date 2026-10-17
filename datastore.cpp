#include "datastore.h"

#include <fstream>
#include <set>

#include "exceptions.h"
#include "utils.h"

DataStore::DataStore(const std::string &filePrefix)
    : profilePath_(filePrefix + "smr_profile.txt"),
      medicinesPath_(filePrefix + "smr_medicines.txt"),
      historyPath_(filePrefix + "smr_history.txt")
{
}

// ------------------------------------------------------------------
// Low-level helpers
// ------------------------------------------------------------------

// Returns false if the file does not exist (that is not an error).
bool DataStore::readLines(const std::string &path, std::vector<std::string> &lines)
{
    lines.clear();

    std::ifstream file(path.c_str());
    if (!file)
        return false;

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')   // Windows line ending
            line.erase(line.size() - 1);
        lines.push_back(line);
    }
    return true;
}

void DataStore::writeLines(const std::string &path, const std::vector<std::string> &lines)
{
    std::ofstream file(path.c_str(), std::ios::trunc);
    if (!file)
        throw FileException("Cannot open \"" + path + "\" for writing. "
                            "Is the folder read-only?");

    for (std::size_t i = 0; i < lines.size(); ++i)
        file << lines[i] << '\n';

    file.flush();
    if (!file)
        throw FileException("Could not save all data to \"" + path + "\" (disk full?).");
}

void DataStore::warn(const std::string &file, std::size_t lineNumber, const std::string &reason)
{
    warnings_.push_back(file + ", line " + std::to_string(lineNumber) + ": " + reason +
                        " (line skipped)");
}

// ------------------------------------------------------------------
// Profile
// ------------------------------------------------------------------
bool DataStore::loadProfile(User &user)
{
    std::vector<std::string> lines;
    if (!readLines(profilePath_, lines))
        return false;

    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        if (util::trim(lines[i]).empty())
            continue;

        const std::vector<std::string> fields = util::split(lines[i], '|');
        int age = 0;
        if (fields.size() != 2 || !util::parseInt(fields[1], age))
        {
            warn(profilePath_, i + 1, "profile line is damaged");
            continue;
        }

        try
        {
            user = User(fields[0], age);
            return true;
        }
        catch (const ValidationException &e)
        {
            warn(profilePath_, i + 1, e.what());
        }
    }
    return false;
}

void DataStore::saveProfile(const User &user)
{
    std::vector<std::string> lines;
    lines.push_back(user.name() + "|" + std::to_string(user.age()));
    writeLines(profilePath_, lines);
}

// ------------------------------------------------------------------
// Medicines
// ------------------------------------------------------------------
std::vector<Reminder> DataStore::loadMedicines()
{
    std::vector<Reminder> medicines;
    std::vector<std::string> lines;
    if (!readLines(medicinesPath_, lines))
        return medicines;   // no file yet = no medicines

    std::set<int> usedIds;

    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        if (util::trim(lines[i]).empty())
            continue;

        const std::vector<std::string> fields = util::split(lines[i], '|');
        int id = 0;
        if (fields.size() != 5 || !util::parseInt(fields[0], id) || id <= 0 ||
            (fields[4] != "0" && fields[4] != "1"))
        {
            warn(medicinesPath_, i + 1, "medicine line is damaged");
            continue;
        }

        if (usedIds.count(id) > 0)
        {
            warn(medicinesPath_, i + 1, "duplicate medicine id");
            continue;
        }

        try
        {
            medicines.push_back(Reminder(id, fields[1], fields[2], fields[3], fields[4] == "1"));
            usedIds.insert(id);
        }
        catch (const ValidationException &e)
        {
            warn(medicinesPath_, i + 1, e.what());
        }
    }
    return medicines;
}

void DataStore::saveMedicines(const std::vector<Reminder> &medicines)
{
    std::vector<std::string> lines;
    for (std::size_t i = 0; i < medicines.size(); ++i)
    {
        const Reminder &m = medicines[i];
        lines.push_back(std::to_string(m.id()) + "|" + m.name() + "|" + m.dosage() + "|" +
                        m.time() + "|" + (m.isEnabled() ? "1" : "0"));
    }
    writeLines(medicinesPath_, lines);
}

// ------------------------------------------------------------------
// Intake history
// ------------------------------------------------------------------
std::vector<IntakeRecord> DataStore::loadHistory()
{
    std::vector<IntakeRecord> records;
    std::vector<std::string> lines;
    if (!readLines(historyPath_, lines))
        return records;

    std::set<std::string> seen;   // "medicineId|date" - one record per dose per day

    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        if (util::trim(lines[i]).empty())
            continue;

        const std::vector<std::string> fields = util::split(lines[i], '|');
        IntakeRecord r;

        const bool shapeOk = fields.size() == 7 &&
                             util::parseInt(fields[0], r.medicineId) &&
                             !util::trim(fields[1]).empty() &&
                             !util::trim(fields[2]).empty() &&
                             util::isValidTime(fields[3]) &&
                             util::isValidDate(fields[4]) &&
                             !util::trim(fields[5]).empty() &&
                             (fields[6] == "TAKEN" || fields[6] == "MISSED");
        if (!shapeOk)
        {
            warn(historyPath_, i + 1, "history line is damaged");
            continue;
        }

        r.medicineName = fields[1];
        r.dosage = fields[2];
        r.scheduledTime = util::normalizeTime(fields[3]);
        r.doseDate = fields[4];
        r.recordedAt = fields[5];
        r.status = (fields[6] == "TAKEN") ? STATUS_TAKEN : STATUS_MISSED;

        if (r.medicineId > 0)
        {
            const std::string key = fields[0] + "|" + r.doseDate;
            if (seen.count(key) > 0)
            {
                warn(historyPath_, i + 1, "duplicate record for the same dose and date");
                continue;
            }
            seen.insert(key);
        }

        records.push_back(r);
    }
    return records;
}

void DataStore::saveHistory(const std::vector<IntakeRecord> &records)
{
    std::vector<std::string> lines;
    for (std::size_t i = 0; i < records.size(); ++i)
    {
        const IntakeRecord &r = records[i];
        lines.push_back(std::to_string(r.medicineId) + "|" + r.medicineName + "|" + r.dosage +
                        "|" + r.scheduledTime + "|" + r.doseDate + "|" + r.recordedAt + "|" +
                        (r.status == STATUS_TAKEN ? "TAKEN" : "MISSED"));
    }
    writeLines(historyPath_, lines);
}
