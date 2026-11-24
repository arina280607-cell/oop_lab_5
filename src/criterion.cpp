#include "criterion.h"
#include "employee.h"

#include <utility>

DepartmentCriterion::DepartmentCriterion(std::string department)
    : department_{std::move(department)}
{
}

bool DepartmentCriterion::matches(const Employee& employee) const
{
    return employee.department() == department_;
}

bool AllCriterion::matches(const Employee&) const
{
    return true;
}