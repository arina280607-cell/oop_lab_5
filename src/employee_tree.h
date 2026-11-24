#pragma once

#include <cstddef>
#include <memory>
#include <ostream>

#include "selection.h"

class Employee;
class Criterion;

class EmployeeTree
{
public:
    EmployeeTree();
    ~EmployeeTree();

    EmployeeTree(const EmployeeTree& other) = delete;
    EmployeeTree& operator=(const EmployeeTree& other) = delete;

    EmployeeTree(EmployeeTree&& other) noexcept;
    EmployeeTree& operator=(EmployeeTree&& other) noexcept;

    bool insert(const Employee& employee);
    bool remove(const Employee& key);

    std::size_t size() const;
    std::size_t height() const;

    std::weak_ptr<const Employee> find(const Employee& key) const;

    void printInOrder(std::ostream& out) const;
    void printReverseOrder(std::ostream& out) const;

    Selection select(const Criterion& criterion) const;

private:
    struct Node;

    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;

    bool insertInto(std::unique_ptr<Node>& link,
                    const Employee& employee);

    bool removeFrom(std::unique_ptr<Node>& link,
                    const Employee& key);

    std::size_t heightOf(const Node* node) const;

    void printInOrderFrom(const Node* node,
                          std::ostream& out) const;

    void printReverseOrderFrom(const Node* node,
                               std::ostream& out) const;

    void selectFrom(const Node* node, const Criterion& criterion, Selection& result) const;
};
