#include "employee.h"
#include <stdexcept>
#include <utility>

Employee::Employee(std::string name, std::string department, int salary)
    : name_{name}, department_{department}, salary_{salary}
{
    if (salary_ < 0)
    {
        throw std::invalid_argument("Salary must be non-negative");
    }
    ++alive_count_;
}

int Employee::salary() const
{
    return salary_;
}

bool Employee::operator<(const Employee& other) const
{
    return salary_ < other.salary_;
}

bool Employee::operator==(const Employee& other) const
{
    return salary_ == other.salary_;
}
std::string Employee::name() const
{
    return name_;
}

std::string Employee::department() const
{
    return department_;
}
Employee::~Employee()
{
    --alive_count_;
}

int Employee::aliveCount()
{
    return alive_count_;
}
Employee::Employee(const Employee& other)
    : name_{other.name_},
      department_{other.department_},
      salary_{other.salary_}
{
    ++alive_count_;
}
Employee::Employee(Employee&& other) noexcept
    : name_{std::move(other.name_)},
      department_{std::move(other.department_)},
      salary_{other.salary_}
{
    ++alive_count_;
}
