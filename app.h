#ifndef APP_H
#define APP_H

#include <set>
#include <string>
#include <vector>

#include "datastore.h"
#include "medicinemanager.h"
#include "report.h"
#include "tracker.h"
#include "user.h"

// ---------------------------------------------------------------
// ConsoleApp  -  the text menu (the console replacement for the
// Qt MainWindow).
//
// It contains NO file code and NO calculations. It only talks to
// the OOP classes:
//     User, MedicineManager (Reminder/Medicine), Tracker, Report
// which in turn use DataStore to save and load.
// ---------------------------------------------------------------
class ConsoleApp
{
public:
    ConsoleApp();

    void run();   // shows the menu until the user chooses Exit

private:
    // ----- start-up and menu -----
    void loadAll();
    void printBanner() const;
    void printMenu() const;

    // ----- profile -----
    void createOrEditProfile();
    void viewProfile() const;

    // ----- medicines -----
    void addMedicine();
    void viewMedicines() const;
    void editMedicine();
    void deleteMedicine();

    // ----- intake tracking and reports -----
    void recordDose();
    void showToday() const;
    void showHistory() const;
    void showReport() const;
    void clearHistory();

    // ----- reminders -----
    void checkReminders(bool showAgain);
    void showHelp() const;

    // ----- helpers -----
    int chooseMedicine(const std::string &prompt) const;   // returns medicine id, 0 = cancelled
    std::string askMedicineText(int field, const std::string &label,
                                const std::string &current) const;
    void printMedicineTable() const;
    std::string statusText(const Reminder &medicine, const std::string &date) const;
    void forgetNotified(int medicineId);
    void showWarnings();

    // ----- the objects that do the real work -----
    DataStore store_;
    User user_;
    MedicineManager medicines_;
    Tracker tracker_;
    Report report_;

    // "medicineId|date" of doses we have already reminded about today.
    std::set<std::string> notified_;
};

#endif // APP_H
