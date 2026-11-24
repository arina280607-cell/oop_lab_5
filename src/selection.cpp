#include "selection.h"

#include <limits>
#include <stdexcept>
#include <utility>

Selection::Selection() = default;
Selection::~Selection() = default;

std::size_t Selection::size() const
{
    return size_;
}

std::size_t Selection::capacity() const
{
    return capacity_;
}

bool Selection::empty() const
{
    return size_ == 0;
}

std::weak_ptr<const Employee> Selection::at(std::size_t index) const
{
    if (index >= size_)
    {
        throw std::out_of_range("Selection index out of range");
    }

    return data_[index];
}

void Selection::pushBack(const std::weak_ptr<const Employee>& employee)
{
    if (size_ == capacity_)
    {
        const std::size_t max_capacity =
            std::numeric_limits<std::size_t>::max()
            / sizeof(std::weak_ptr<const Employee>);
        if (capacity_ > max_capacity / 2)
        {
            throw std::length_error("Selection capacity overflow");
        }

        const std::size_t new_capacity =
            capacity_ == 0 ? 1 : capacity_ * 2;

        auto new_data =
            std::make_unique<std::weak_ptr<const Employee>[]>(new_capacity);

        for (std::size_t i = 0; i < size_; ++i)
        {
            new_data[i] = data_[i];
        }

        data_ = std::move(new_data);
        capacity_ = new_capacity;
    }

    data_[size_] = employee;
    ++size_;
}
Selection::Selection(const Selection& other)//констр копирования 
    : size_{other.size_},
      capacity_{other.capacity_}
{
    if (capacity_ != 0)
    {
        data_ =
            std::make_unique<std::weak_ptr<const Employee>[]>(capacity_);

        for (std::size_t i = 0; i < size_; ++i)
        {
            data_[i] = other.data_[i];
        }
    }
}
void Selection::swap(Selection& other) noexcept
{
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}
Selection& Selection::operator=(const Selection& other)//копирующее присваивание
{
    if (this != &other)
    {
        Selection copy{other};
        swap(copy);
    }

    return *this;
}
Selection::Selection(Selection&& other) noexcept
    : data_{std::move(other.data_)},
      size_{std::exchange(other.size_, 0)},
      capacity_{std::exchange(other.capacity_, 0)}
{
}
Selection& Selection::operator=(Selection&& other) noexcept
{
    if (this != &other)
    {
        data_ = std::move(other.data_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    return *this;
}
