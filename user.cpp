#include "user.h"

User::User(const std::string &name, int age)
{
    setName(name);
    setAge(age);
}

bool User::hasProfile() const
{
    return !name().empty() && age() > 0;
}

std::string User::displayProfile() const
{
    if (!hasProfile())
        return "No profile created yet.";

    return "Name : " + name() + "\nAge  : " + std::to_string(age()) + " years";
}

std::string User::role() const
{
    return "Patient";
}
