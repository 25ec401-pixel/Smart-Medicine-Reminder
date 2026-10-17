#ifndef USER_H
#define USER_H

#include <string>

#include "person.h"

// ---------------------------------------------------------------
// User  -  Inheritance + Polymorphism
//
// User IS-A Person. It writes the pure virtual functions
// displayProfile() and role() that Person only declared.
// ---------------------------------------------------------------
class User : public Person
{
public:
    User() {}
    User(const std::string &name, int age);   // throws ValidationException

    // True once a valid name and age have been saved.
    bool hasProfile() const;

    // Overrides of Person's pure virtual functions.
    std::string displayProfile() const;
    std::string role() const;
};

#endif // USER_H
