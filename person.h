#ifndef PERSON_H
#define PERSON_H

#include <string>

// ---------------------------------------------------------------
// Person  -  ABSTRACT base class   (Abstraction)
//
// It has pure virtual functions ("= 0"), so you cannot create a
// Person object directly. A child class (like User) must provide
// the missing functions.
//
// Encapsulation: name_ and age_ are private. They can only be
// changed through setName() and setAge(), which check the input.
// ---------------------------------------------------------------
class Person
{
public:
    virtual ~Person() {}

    const std::string &name() const { return name_; }
    int age() const { return age_; }

    void setName(const std::string &name);   // throws ValidationException
    void setAge(int age);                    // throws ValidationException

    // Pure virtual functions -> every child class must write them.
    virtual std::string displayProfile() const = 0;
    virtual std::string role() const = 0;

protected:
    Person() : age_(0) {}

private:
    std::string name_;
    int age_;
};

#endif // PERSON_H
