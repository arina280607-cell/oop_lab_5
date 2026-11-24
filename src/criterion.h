#pragma once

#include <string>

class Employee;

class Criterion
{
public:
    virtual ~Criterion() = default;

    virtual bool matches(const Employee& employee) const = 0;
};

class DepartmentCriterion : public Criterion
{
public:
    explicit DepartmentCriterion(std::string department);

    bool matches(const Employee& employee) const override;

private:
    std::string department_;
};

class AllCriterion : public Criterion
{
public:
    bool matches(const Employee& employee) const override;
};