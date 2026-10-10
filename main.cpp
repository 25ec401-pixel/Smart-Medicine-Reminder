#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
using namespace std;

// Abstract Class
class Person
{
public:
    virtual void displayProfile() = 0;
};

// User Class
class User : public Person
{
private:
    string name;
    int age;

public:
    void createProfile()
    {
        cout << "\nEnter Name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter Age: ";
        cin >> age;
    }

    void displayProfile()
    {
        cout << "\n===== USER PROFILE =====";
        cout << "\nName : " << name;
        cout << "\nAge  : " << age << endl;
    }
};

// Medicine Class
class Medicine
{
protected:
    string medName;
    string dosage;
    string time;

public:
    void addMedicine()
    {
        cin.ignore();

        cout << "\nMedicine Name: ";
        getline(cin, medName);

        cout << "Dosage: ";
        getline(cin, dosage);

        cout << "Time (HH:MM): ";
        getline(cin, time);

        ofstream file("medicines.txt", ios::app);
        file << medName << "," << dosage << "," << time << endl;
        file.close();

        cout << "\nMedicine Added Successfully!\n";
    }

    void displayMedicine()
    {
        cout << left
             << setw(20) << medName
             << setw(15) << dosage
             << setw(10) << time << endl;
    }
};

// Reminder Class (Inheritance)
class Reminder : public Medicine
{
public:
    void setReminder()
    {
        cout << "\nReminder Set For Medicine at "
             << time << endl;
    }
};

// Tracker Class
class Tracker
{
private:
    int taken = 0;
    int missed = 0;

public:
    void markTaken()
    {
        taken++;

        ofstream file("history.txt", ios::app);
        file << "TAKEN\n";
        file.close();

        cout << "\nMedicine Marked as Taken!\n";
    }

    void markMissed()
    {
        missed++;

        ofstream file("history.txt", ios::app);
        file << "MISSED\n";
        file.close();

        cout << "\nMedicine Marked as Missed!\n";
    }

    int getTaken()
    {
        return taken;
    }

    int getMissed()
    {
        return missed;
    }
};

// Report Class
class Report
{
public:
    void generateReport(int taken, int missed)
    {
        int total = taken + missed;

        cout << "\n===== HEALTH REPORT =====";
        cout << "\nTaken Doses  : " << taken;
        cout << "\nMissed Doses : " << missed;

        if (total > 0)
        {
            double adherence =
                ((double)taken / total) * 100;

            cout << "\nAdherence %  : "
                 << fixed << setprecision(2)
                 << adherence << "%";
        }

        cout << endl;
    }
};

// Function to View Medicines
void viewMedicines()
{
    ifstream file("medicines.txt");

    string name, dosage, time;

    cout << "\n===== MEDICINE LIST =====\n";

    cout << left
         << setw(20) << "Medicine"
         << setw(15) << "Dosage"
         << setw(10) << "Time"
         << endl;

    while (getline(file, name, ',') &&
           getline(file, dosage, ',') &&
           getline(file, time))
    {
        cout << left
             << setw(20) << name
             << setw(15) << dosage
             << setw(10) << time
             << endl;
    }

    file.close();
}

// Main
int main()
{
    User user;
    Medicine medicine;
    Reminder reminder;
    Tracker tracker;
    Report report;

    int choice;

    do
    {
        cout << "\n\n";
        cout << "==============================\n";
        cout << " SMART MEDICINE REMINDER\n";
        cout << "==============================\n";

        cout << "1. Create Profile\n";
        cout << "2. View Profile\n";
        cout << "3. Add Medicine\n";
        cout << "4. View Medicines\n";
        cout << "5. Mark Medicine Taken\n";
        cout << "6. Mark Medicine Missed\n";
        cout << "7. Generate Report\n";
        cout << "8. Exit\n";

        cout << "\nEnter Choice: ";
        cin >> choice;

        try
        {
            switch (choice)
            {
            case 1:
                user.createProfile();
                break;

            case 2:
                user.displayProfile();
                break;

            case 3:
                medicine.addMedicine();
                break;

            case 4:
                viewMedicines();
                break;

            case 5:
                tracker.markTaken();
                break;

            case 6:
                tracker.markMissed();
                break;

            case 7:
                report.generateReport(
                    tracker.getTaken(),
                    tracker.getMissed());
                break;

            case 8:
                cout << "\nThank You!\n";
                break;

            default:
                throw choice;
            }
        }
        catch (...)
        {
            cout << "\nInvalid Choice!\n";
        }

    } while (choice != 8);

    return 0;
}
