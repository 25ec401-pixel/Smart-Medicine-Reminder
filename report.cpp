#include "report.h"

#include <map>

double Report::calculateAdherence(int taken, int missed)
{
    const int total = taken + missed;
    if (total <= 0)
        return 0.0;   // avoids dividing by zero when nothing is recorded

    return (static_cast<double>(taken) / total) * 100.0;
}

ReportData Report::generate() const
{
    ReportData data;
    data.taken = tracker_.countStatus(STATUS_TAKEN);
    data.missed = tracker_.countStatus(STATUS_MISSED);
    data.total = data.taken + data.missed;
    data.hasData = data.total > 0;
    data.adherence = calculateAdherence(data.taken, data.missed);
    return data;
}

std::vector<MedicineStats> Report::perMedicine() const
{
    std::map<std::string, MedicineStats> byName;   // a map keeps the names sorted

    const std::vector<IntakeRecord> &records = tracker_.records();
    for (std::size_t i = 0; i < records.size(); ++i)
    {
        MedicineStats &stats = byName[records[i].medicineName];
        stats.name = records[i].medicineName;
        if (records[i].status == STATUS_TAKEN)
            ++stats.taken;
        else
            ++stats.missed;
    }

    std::vector<MedicineStats> result;
    for (std::map<std::string, MedicineStats>::const_iterator it = byName.begin();
         it != byName.end(); ++it)
    {
        result.push_back(it->second);
    }
    return result;
}

std::string Report::message(const ReportData &data)
{
    if (!data.hasData)
        return "No doses recorded yet. Record doses as Taken or Missed to see your adherence.";

    if (data.adherence >= 90.0)
        return "Excellent! You are following your medicine schedule very well.";
    if (data.adherence >= 75.0)
        return "Good job. A few doses were missed - try to stay consistent.";
    if (data.adherence >= 50.0)
        return "Fair. Keeping your medicines in sight can help you remember.";

    return "Low adherence. Try to take your medicines on time, and talk to your doctor if you face problems.";
}
