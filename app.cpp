#include "app.h"

#include <iostream>

#include "exceptions.h"
#include "utils.h"

using std::cout;
using std::string;
using std::vector;

namespace
{
    // Which medicine field askMedicineText() should check.
    const int FIELD_NAME = 1;
    const int FIELD_DOSAGE = 2;
    const int FIELD_TIME = 3;

    void printTitle(const string &title)
    {
        cout << "\n";
        util::printLine('=', 60);
        cout << "  " << title << "\n";
        util::printLine('=', 60);
    }
}

ConsoleApp::ConsoleApp()
    : store_(),
      user_(),
      medicines_(store_),
      tracker_(store_),
      report_(tracker_)
{
}

// =====================================================================
// Start-up and main loop
// =====================================================================
void ConsoleApp::loadAll()
{
    store_.loadProfile(user_);   // false = no profile yet, which is fine
    medicines_.load();
    tracker_.load();
}

void ConsoleApp::showWarnings()
{
    const vector<string> &warnings = store_.warnings();
    if (warnings.empty())
        return;

    cout << "\nNote: some saved data could not be read and was skipped:\n";
    for (std::size_t i = 0; i < warnings.size(); ++i)
        cout << "  - " << warnings[i] << "\n";
    store_.clearWarnings();
}

void ConsoleApp::printBanner() const
{
    cout << "\n";
    util::printLine('=', 60);
    cout << "              SMART MEDICINE REMINDER  (Console)\n";
    util::printLine('=', 60);

    if (user_.hasProfile())
        cout << "  Welcome back, " << user_.name() << "!\n";
    else
        cout << "  Welcome! Start with option 1 to create your profile.\n";

    cout << "  Today: " << util::today() << "   Time: "
         << util::formatTime12(util::currentTime()) << "\n";
}

void ConsoleApp::printMenu() const
{
    cout << "\n";
    util::printLine('-', 60);
    cout << "   1. Create / edit profile       8. Today's schedule\n"
            "   2. View profile                9. Intake history\n"
            "   3. Add medicine               10. Health report\n"
            "   4. View medicines             11. Check reminders now\n"
            "   5. Edit medicine              12. Clear intake history\n"
            "   6. Delete medicine            13. Help / about reminders\n"
            "   7. Record dose (taken/missed)  0. Exit\n";
    util::printLine('-', 60);
}

void ConsoleApp::run()
{
    loadAll();        // load first, so the banner knows whether a profile exists
    printBanner();
    showWarnings();

    while (true)
    {
        try
        {
            checkReminders(false);   // reminders appear whenever we come back to the menu
            printMenu();

            const int choice = util::readInt("Enter your choice: ", 0, 13);
            if (choice == 0)
                break;

            switch (choice)
            {
            case 1:  createOrEditProfile(); break;
            case 2:  viewProfile();         break;
            case 3:  addMedicine();         break;
            case 4:  viewMedicines();       break;
            case 5:  editMedicine();        break;
            case 6:  deleteMedicine();      break;
            case 7:  recordDose();          break;
            case 8:  showToday();           break;
            case 9:  showHistory();         break;
            case 10: showReport();          break;
            case 11: checkReminders(true);  break;
            case 12: clearHistory();        break;
            case 13: showHelp();            break;
            default: break;   // cannot happen: readInt only returns 0..13
            }

            util::pause();
        }
        catch (const ValidationException &e)
        {
            cout << "\n  Input error: " << e.what() << "\n";
            util::pause();
        }
        catch (const FileException &e)
        {
            cout << "\n  File error: " << e.what() << "\n"
                 << "  Your last change may NOT have been saved.\n";
            util::pause();
        }
    }

    cout << "\nThank you for using Smart Medicine Reminder. Stay healthy!\n";
    cout << "Remember: reminders only work while this program is open.\n";
}

// =====================================================================
// Profile
// =====================================================================
void ConsoleApp::createOrEditProfile()
{
    const bool editing = user_.hasProfile();
    printTitle(editing ? "EDIT PROFILE" : "CREATE PROFILE");
    if (editing)
        cout << "  (press Enter to keep the current value)\n";

    User updated = user_;   // work on a copy; keep it only if saving works

    // ----- name -----
    while (true)
    {
        const string prompt = editing ? "Name [" + user_.name() + "]: " : "Name: ";
        const string text = util::readLine(prompt);
        if (editing && text.empty())
            break;
        try
        {
            updated.setName(text);
            break;
        }
        catch (const ValidationException &e)
        {
            cout << "  " << e.what() << "\n";
        }
    }

    // ----- age -----
    while (true)
    {
        const string prompt = editing ? "Age [" + std::to_string(user_.age()) + "]: " : "Age: ";
        const string text = util::readLine(prompt);
        if (editing && text.empty())
            break;

        int age = 0;
        if (!util::parseInt(text, age))
        {
            cout << "  Please enter your age as a whole number.\n";
            continue;
        }
        try
        {
            updated.setAge(age);
            break;
        }
        catch (const ValidationException &e)
        {
            cout << "  " << e.what() << "\n";
        }
    }

    store_.saveProfile(updated);   // may throw FileException
    user_ = updated;
    cout << "\n  Profile saved successfully.\n";
}

void ConsoleApp::viewProfile() const
{
    printTitle("USER PROFILE");
    cout << user_.displayProfile() << "\n";   // polymorphic call through Person
    if (user_.hasProfile())
        cout << "Role : " << user_.role() << "\n";
}

// =====================================================================
// Medicines
// =====================================================================
string ConsoleApp::askMedicineText(int field, const string &label, const string &current) const
{
    while (true)
    {
        const string prompt = current.empty() ? label + ": " : label + " [" + current + "]: ";
        const string text = util::readLine(prompt);

        if (text.empty() && !current.empty())
            return current;   // Enter keeps the old value (edit mode)

        try
        {
            Medicine check;   // let the Medicine setters validate the text
            if (field == FIELD_NAME)
            {
                check.setName(text);
                return check.name();
            }
            if (field == FIELD_DOSAGE)
            {
                check.setDosage(text);
                return check.dosage();
            }
            check.setTime(text);
            return check.time();
        }
        catch (const ValidationException &e)
        {
            cout << "  " << e.what() << "\n";
        }
    }
}

void ConsoleApp::addMedicine()
{
    printTitle("ADD MEDICINE");

    const string name = askMedicineText(FIELD_NAME, "Medicine name", "");
    const string dosage = askMedicineText(FIELD_DOSAGE, "Dosage (e.g. 500 mg, 1 tablet)", "");
    const string time = askMedicineText(FIELD_TIME, "Daily time (24-hour HH:MM, e.g. 08:30)", "");
    const bool reminderOn = util::readYesNo("Remind me at this time? (y/n): ");

    medicines_.add(name, dosage, time, reminderOn);   // validates and saves
    cout << "\n  " << name << " was added successfully.\n";
}

void ConsoleApp::printMedicineTable() const
{
    const vector<Reminder> &list = medicines_.all();

    cout << util::padRight(" No", 5) << util::padRight("Medicine", 26)
         << util::padRight("Dosage", 16) << util::padRight("Time", 11) << "Reminder\n";
    util::printLine('-', 66);

    for (std::size_t i = 0; i < list.size(); ++i)
    {
        cout << util::padRight(" " + std::to_string(i + 1), 5)
             << util::padRight(util::fit(list[i].name(), 24), 26)
             << util::padRight(util::fit(list[i].dosage(), 14), 16)
             << util::padRight(util::formatTime12(list[i].time()), 11)
             << (list[i].isEnabled() ? "On" : "Off") << "\n";
    }
}

void ConsoleApp::viewMedicines() const
{
    printTitle("MEDICINE LIST");

    if (medicines_.count() == 0)
    {
        cout << "  No medicines saved yet. Use option 3 to add one.\n";
        return;
    }

    printMedicineTable();
    cout << "\nTotal medicines: " << medicines_.count() << "\n";
}

int ConsoleApp::chooseMedicine(const string &prompt) const
{
    if (medicines_.count() == 0)
    {
        cout << "  No medicines saved yet. Use option 3 to add one.\n";
        return 0;
    }

    printMedicineTable();
    cout << "\n";

    const int number = util::readInt(prompt, 0, static_cast<int>(medicines_.count()));
    if (number == 0)
        return 0;

    return medicines_.all()[static_cast<std::size_t>(number - 1)].id();
}

void ConsoleApp::editMedicine()
{
    printTitle("EDIT MEDICINE");

    const int id = chooseMedicine("Enter the number of the medicine to edit (0 = cancel): ");
    if (id == 0)
        return;

    const Reminder *found = medicines_.find(id);
    if (found == nullptr)
        throw ValidationException("That medicine no longer exists.");
    const Reminder current = *found;   // copy: the list changes when we save

    cout << "\n  Editing: " << current.summary() << "\n"
         << "  (press Enter to keep the current value)\n\n";

    const string name = askMedicineText(FIELD_NAME, "Medicine name", current.name());
    const string dosage = askMedicineText(FIELD_DOSAGE, "Dosage", current.dosage());
    const string time = askMedicineText(FIELD_TIME, "Daily time (HH:MM)", current.time());

    bool reminderOn = current.isEnabled();
    while (true)
    {
        const string answer = util::toUpper(util::readLine(
            string("Reminder on? (y/n) [") + (reminderOn ? "y" : "n") + "]: "));
        if (answer.empty())
            break;
        if (answer == "Y" || answer == "YES")
        {
            reminderOn = true;
            break;
        }
        if (answer == "N" || answer == "NO")
        {
            reminderOn = false;
            break;
        }
        cout << "  Please type Y or N.\n";
    }

    medicines_.edit(id, name, dosage, time, reminderOn);   // validates and saves
    forgetNotified(id);   // allow a fresh reminder if the time was changed
    cout << "\n  " << name << " was updated successfully.\n";
}

void ConsoleApp::deleteMedicine()
{
    printTitle("DELETE MEDICINE");

    const int id = chooseMedicine("Enter the number of the medicine to delete (0 = cancel): ");
    if (id == 0)
        return;

    const Reminder *found = medicines_.find(id);
    if (found == nullptr)
        throw ValidationException("That medicine no longer exists.");
    const string name = found->name();

    cout << "\n  Its past dose history will be kept in the history list.\n";
    if (!util::readYesNo("  Delete \"" + name + "\"? (y/n): "))
    {
        cout << "  Nothing was deleted.\n";
        return;
    }

    medicines_.remove(id);
    forgetNotified(id);
    tracker_.detachMedicine(id);
    cout << "\n  " << name << " was deleted.\n";
}

// =====================================================================
// Recording doses, history, report
// =====================================================================
string ConsoleApp::statusText(const Reminder &medicine, const string &date) const
{
    IntakeStatus status = STATUS_TAKEN;
    if (tracker_.find(medicine.id(), date, status))
        return statusToText(status);

    if (date == util::today() && medicine.isDue(util::currentTime()))
        return "DUE NOW";
    return "Pending";
}

void ConsoleApp::recordDose()
{
    printTitle("RECORD A DOSE");

    const int id = chooseMedicine("Enter the number of the medicine (0 = cancel): ");
    if (id == 0)
        return;

    const Reminder *found = medicines_.find(id);
    if (found == nullptr)
        throw ValidationException("That medicine no longer exists.");
    const Reminder medicine = *found;

    string date = util::readLine("Dose date YYYY-MM-DD (press Enter for today): ");
    if (date.empty())
        date = util::today();

    cout << "  1. Taken\n  2. Missed\n";
    const int choice = util::readInt("Was the dose taken or missed? ", 1, 2);
    const IntakeStatus status = (choice == 1) ? STATUS_TAKEN : STATUS_MISSED;

    const bool replaced = tracker_.record(medicine, date, status);   // validates and saves

    cout << "\n  " << medicine.name() << " marked as " << statusToText(status)
         << " for " << date << ".\n";
    if (replaced)
        cout << "  (the earlier record for that day was updated)\n";
}

void ConsoleApp::showToday() const
{
    printTitle("TODAY'S SCHEDULE  (" + util::today() + ")");

    if (medicines_.count() == 0)
    {
        cout << "  No medicines saved yet. Use option 3 to add one.\n";
        return;
    }

    const string date = util::today();
    const vector<Reminder> &list = medicines_.all();
    int taken = 0;
    int missed = 0;
    int pending = 0;

    cout << util::padRight(" No", 5) << util::padRight("Medicine", 26)
         << util::padRight("Dosage", 16) << util::padRight("Time", 11) << "Status\n";
    util::printLine('-', 66);

    for (std::size_t i = 0; i < list.size(); ++i)
    {
        const string status = statusText(list[i], date);
        if (status == "Taken")
            ++taken;
        else if (status == "Missed")
            ++missed;
        else
            ++pending;

        cout << util::padRight(" " + std::to_string(i + 1), 5)
             << util::padRight(util::fit(list[i].name(), 24), 26)
             << util::padRight(util::fit(list[i].dosage(), 14), 16)
             << util::padRight(util::formatTime12(list[i].time()), 11)
             << status << "\n";
    }

    cout << "\nTaken: " << taken << "   Missed: " << missed << "   Pending: " << pending << "\n";
}

void ConsoleApp::showHistory() const
{
    printTitle("INTAKE HISTORY");

    const vector<IntakeRecord> history = tracker_.newestFirst();
    if (history.empty())
    {
        cout << "  No doses recorded yet.\n";
        return;
    }

    cout << util::padRight("Date", 12) << util::padRight("Sched.", 10)
         << util::padRight("Medicine", 26) << util::padRight("Dosage", 14)
         << util::padRight("Status", 8) << "Recorded at\n";
    util::printLine('-', 84);

    for (std::size_t i = 0; i < history.size(); ++i)
    {
        const IntakeRecord &r = history[i];
        const string name = r.medicineName + (r.medicineId == 0 ? " (deleted)" : "");

        cout << util::padRight(r.doseDate, 12)
             << util::padRight(util::formatTime12(r.scheduledTime), 10)
             << util::padRight(util::fit(name, 24), 26)
             << util::padRight(util::fit(r.dosage, 12), 14)
             << util::padRight(statusToText(r.status), 8)
             << r.recordedAt << "\n";
    }
    cout << "\nTotal records: " << history.size() << "\n";
}

void ConsoleApp::showReport() const
{
    printTitle("HEALTH REPORT");

    const ReportData data = report_.generate();

    cout << "  Taken doses    : " << data.taken << "\n"
         << "  Missed doses   : " << data.missed << "\n"
         << "  Total recorded : " << data.total << "\n";

    if (data.hasData)
    {
        cout << "  Adherence      : " << util::percentText(data.adherence) << "  "
             << util::barText(data.adherence, 20) << "\n";
    }
    else
    {
        cout << "  Adherence      : N/A (no doses recorded yet)\n";
    }

    cout << "\n  " << Report::message(data) << "\n";
    cout << "\n  Adherence (%) = (Taken Doses / Total Recorded Doses) x 100\n";

    const vector<MedicineStats> stats = report_.perMedicine();
    if (!stats.empty())
    {
        cout << "\n  By medicine:\n";
        cout << "  " << util::padRight("Medicine", 26) << util::padRight("Taken", 8)
             << util::padRight("Missed", 8) << "Adherence\n";
        cout << "  ";
        util::printLine('-', 56);
        for (std::size_t i = 0; i < stats.size(); ++i)
        {
            cout << "  " << util::padRight(util::fit(stats[i].name, 24), 26)
                 << util::padRight(std::to_string(stats[i].taken), 8)
                 << util::padRight(std::to_string(stats[i].missed), 8)
                 << util::percentText(Report::calculateAdherence(stats[i].taken, stats[i].missed))
                 << "\n";
        }
    }
}

void ConsoleApp::clearHistory()
{
    printTitle("CLEAR INTAKE HISTORY");

    if (tracker_.records().empty())
    {
        cout << "  There is no history to clear.\n";
        return;
    }

    cout << "  This deletes ALL " << tracker_.records().size()
         << " recorded doses and resets the report. It cannot be undone.\n";
    if (!util::readYesNo("  Are you sure? (y/n): "))
    {
        cout << "  Nothing was deleted.\n";
        return;
    }

    tracker_.clear();
    cout << "\n  Intake history cleared.\n";
}

// =====================================================================
// Reminders
// =====================================================================
void ConsoleApp::forgetNotified(int medicineId)
{
    const string prefix = std::to_string(medicineId) + "|";

    std::set<string>::iterator it = notified_.begin();
    while (it != notified_.end())
    {
        if (it->compare(0, prefix.size(), prefix) == 0)
            it = notified_.erase(it);
        else
            ++it;
    }
}

// A console program cannot interrupt you while it waits for typing, so
// reminders are checked here: when the program starts, every time the menu
// is shown, and when you choose option 11.
//   showAgain == false : ask only about doses we have not mentioned yet
//   showAgain == true  : ask about every due dose again (option 11)
void ConsoleApp::checkReminders(bool showAgain)
{
    const string date = util::today();
    const string now = util::currentTime();

    vector<Reminder> toAsk;
    int stillPending = 0;

    const vector<Reminder> &list = medicines_.all();
    for (std::size_t i = 0; i < list.size(); ++i)
    {
        IntakeStatus recorded = STATUS_TAKEN;
        if (!list[i].isDue(now) || tracker_.find(list[i].id(), date, recorded))
            continue;   // not due yet, reminder off, or already recorded today

        const string key = std::to_string(list[i].id()) + "|" + date;
        if (showAgain || notified_.count(key) == 0)
            toAsk.push_back(list[i]);
        else
            ++stillPending;
    }

    if (toAsk.empty())
    {
        if (showAgain)
            cout << "\n  No reminders are due right now. You are all caught up!\n";
        else if (stillPending > 0)
            cout << "\n  >> " << stillPending << " dose(s) are still pending today. "
                 << "Use option 11 or 7 to record them.\n";
        return;
    }

    for (std::size_t i = 0; i < toAsk.size(); ++i)
    {
        notified_.insert(std::to_string(toAsk[i].id()) + "|" + date);   // remember: no duplicates

        cout << "\n";
        util::printLine('*', 60);
        cout << "  *** MEDICINE REMINDER ***\n";
        cout << "  " << toAsk[i].notificationText() << "\n";
        util::printLine('*', 60);
        cout << "  1. Mark as Taken\n  2. Mark as Missed\n  3. Decide later\n";

        const int choice = util::readInt("Your choice: ", 1, 3);
        if (choice == 3)
        {
            cout << "  OK. It stays pending - you can record it with option 7.\n";
            continue;
        }

        const IntakeStatus status = (choice == 1) ? STATUS_TAKEN : STATUS_MISSED;
        try
        {
            tracker_.record(toAsk[i], date, status);
            cout << "  Recorded: " << toAsk[i].name() << " - " << statusToText(status) << ".\n";
        }
        catch (const ValidationException &e)
        {
            cout << "  Input error: " << e.what() << "\n";
        }
        catch (const FileException &e)
        {
            cout << "  File error: " << e.what() << "\n";
        }
    }
}

void ConsoleApp::showHelp() const
{
    printTitle("HELP - HOW REMINDERS WORK");

    cout << "  This is a console program, so it is different from the Qt\n"
            "  desktop version:\n\n"
            "  * It cannot pop up a window or run in the background.\n"
            "  * Reminders are checked when the program starts, every time\n"
            "    the main menu is shown, and when you choose option 11.\n"
            "  * If the program is CLOSED (or the computer is off), no\n"
            "    reminder can appear. Next time you open it, every dose\n"
            "    whose time has already passed today is shown as DUE.\n"
            "  * While you are typing at a prompt, the reminder waits until\n"
            "    you return to the menu.\n"
            "  * Each due dose is announced once per day; choose\n"
            "    \"Decide later\" and use option 11 or 7 to record it.\n\n"
            "  Your data is saved automatically in these files (in the\n"
            "  program's working folder):\n"
            "    " << store_.profilePath() << "\n"
            "    " << store_.medicinesPath() << "\n"
            "    " << store_.historyPath() << "\n";
}
