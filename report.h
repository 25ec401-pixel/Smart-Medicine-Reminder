#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>

#include "tracker.h"

// Numbers shown on the health report.
struct ReportData
{
    int taken;
    int missed;
    int total;          // taken + missed
    double adherence;   // percentage 0..100
    bool hasData;       // false when no dose has been recorded yet

    ReportData() : taken(0), missed(0), total(0), adherence(0.0), hasData(false) {}
};

// Taken/missed numbers for one medicine name.
struct MedicineStats
{
    std::string name;
    int taken;
    int missed;

    MedicineStats() : taken(0), missed(0) {}
};

// ---------------------------------------------------------------
// Report  -  builds the health report from the saved history
//            (not from temporary counters).
//
//   Adherence (%) = (Taken Doses / Total Recorded Doses) x 100
// ---------------------------------------------------------------
class Report
{
public:
    explicit Report(const Tracker &tracker) : tracker_(tracker) {}

    ReportData generate() const;
    std::vector<MedicineStats> perMedicine() const;

    static double calculateAdherence(int taken, int missed);
    static std::string message(const ReportData &data);

private:
    const Tracker &tracker_;   // read only
};

#endif // REPORT_H
