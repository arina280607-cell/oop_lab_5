#pragma once
#include <string>

class Employee
{
private:
    std::string name_;
    std::string department_;
    int salary_;
    inline static int alive_count_ = 0;

public:
    Employee(std::string name, std::string department, int salary);
    int salary() const;
    std::string name() const;
    std::string department() const;
    bool operator<(const Employee &other) const;
    bool operator==(const Employee &other) const;
    Employee& operator=(const Employee& other) = default;//копирующее присваивание
    Employee& operator=(Employee&& other) noexcept = default;//перемещающее присваивание
    Employee(const Employee& other); //конструкор копирования
    Employee(Employee&& other) noexcept;//конструкор перемещения
    ~Employee();
    static int aliveCount();
};