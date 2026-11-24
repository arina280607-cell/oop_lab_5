#pragma once

#include <cstddef>
#include <memory>

class Employee;

class Selection
{
public:
    Selection();
    ~Selection();

    Selection(const Selection& other);
    Selection& operator=(const Selection& other);

    Selection(Selection&& other) noexcept;
    Selection& operator=(Selection&& other) noexcept;

    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;

    void pushBack(const std::weak_ptr<const Employee>& employee);

    std::weak_ptr<const Employee> at(std::size_t index) const;

private:
    std::unique_ptr<std::weak_ptr<const Employee>[]> data_;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;

    void swap(Selection& other) noexcept;
};
