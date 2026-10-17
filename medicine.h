#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>

// ---------------------------------------------------------------
// Medicine  -  stores the details of one medicine.
//
// Encapsulation : data members are private; setters check input.
// Polymorphism  : summary() is virtual, so a child class
//                 (Reminder) can change how a medicine describes itself.
// ---------------------------------------------------------------
class Medicine
{
public:
    Medicine() : id_(0) {}
    Medicine(int id, const std::string &name, const std::string &dosage,
             const std::string &time);   // throws ValidationException
    virtual ~Medicine() {}

    int id() const { return id_; }
    const std::string &name() const { return name_; }
    const std::string &dosage() const { return dosage_; }
    const std::string &time() const { return time_; }   // always "HH:MM" (24-hour)

    void setId(int id) { id_ = id; }
    void setName(const std::string &name);       // throws ValidationException
    void setDosage(const std::string &dosage);   // throws ValidationException
    void setTime(const std::string &time);       // throws ValidationException

    // Virtual: Reminder overrides this to add reminder information.
    virtual std::string summary() const;

private:
    int id_;               // 0 = not saved yet
    std::string name_;
    std::string dosage_;
    std::string time_;
};

#endif // MEDICINE_H
