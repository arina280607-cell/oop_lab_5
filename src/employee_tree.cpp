#include "employee_tree.h"
#include "employee.h"
#include <algorithm>
#include <utility>
#include "criterion.h"

struct EmployeeTree::Node
{
    std::shared_ptr<Employee> data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit Node(const Employee& employee)
        : data{std::make_shared<Employee>(employee)}
    {
    }
};

EmployeeTree::EmployeeTree() = default;
EmployeeTree::~EmployeeTree() = default;

EmployeeTree::EmployeeTree(EmployeeTree&& other) noexcept
    : root_{std::move(other.root_)},
      size_{std::exchange(other.size_, 0)}
{
}

EmployeeTree& EmployeeTree::operator=(EmployeeTree&& other) noexcept
{
    if (this != &other)
    {
        root_ = std::move(other.root_);
        size_ = std::exchange(other.size_, 0);
    }

    return *this;
}


std::size_t EmployeeTree::size() const
{
    return size_;
}

bool EmployeeTree::insert(const Employee& employee)
{
    if (insertInto(root_, employee))
    {
        ++size_;
        return true;
    }

    return false;
}

bool EmployeeTree::insertInto(std::unique_ptr<Node>& link, const Employee& employee)
{
    if (!link)
    {
        link = std::make_unique<Node>(employee);
        return true;
    }

    if (employee < *link->data)
    {
        return insertInto(link->left, employee);
    }

    if (*link->data < employee)
    {
        return insertInto(link->right, employee);
    }

    return false;
}

std::weak_ptr<const Employee> EmployeeTree::find(const Employee& key) const
{
    const Node* current = root_.get();

    while (current != nullptr)
    {
        if (key < *current->data)
        {
            current = current->left.get();
        }
        else if (*current->data < key)
        {
            current = current->right.get();
        }
        else
        {
            return current->data;
        }
    }

    return {};
}

std::size_t EmployeeTree::height() const
{
    return heightOf(root_.get());
}

std::size_t EmployeeTree::heightOf(const Node* node) const
{
    if (node == nullptr)
    {
        return 0;
    }

    const std::size_t left_height = heightOf(node->left.get());
    const std::size_t right_height = heightOf(node->right.get());

    return 1 + std::max(left_height, right_height);
}
void EmployeeTree::printInOrder(std::ostream& out) const
{
    printInOrderFrom(root_.get(), out);
}

void EmployeeTree::printInOrderFrom(const Node* node, std::ostream& out) const
{
    if (node == nullptr)
    {
        return;
    }

    printInOrderFrom(node->left.get(), out);

    out << node->data->name() << ' '
        << node->data->department() << ' '
        << node->data->salary() << '\n';

    printInOrderFrom(node->right.get(), out);
}
bool EmployeeTree::remove(const Employee& key)
{
    if (removeFrom(root_, key))
    {
        --size_;
        return true;
    }

    return false;
}

bool EmployeeTree::removeFrom(std::unique_ptr<Node>& link,
                              const Employee& key)
{
    if (!link)
    {
        return false;
    }

    if (key < *link->data)
    {
        return removeFrom(link->left, key);
    }

    if (*link->data < key)
    {
        return removeFrom(link->right, key);
    }

    if (!link->left)
    {
        auto replacement = std::move(link->right);
        link = std::move(replacement);
        return true;
    }

    if (!link->right)
    {
        auto replacement = std::move(link->left);
        link = std::move(replacement);
        return true;
    }

    auto* successor_link = &link->right;

    while ((*successor_link)->left)
    {
        successor_link = &(*successor_link)->left;
    }

    link->data = std::move((*successor_link)->data);

    auto replacement = std::move((*successor_link)->right);
    *successor_link = std::move(replacement);

    return true;
}
void EmployeeTree::printReverseOrder(std::ostream& out) const
{
    printReverseOrderFrom(root_.get(), out);
}

void EmployeeTree::printReverseOrderFrom(const Node* node,
                                         std::ostream& out) const
{
    if (node == nullptr)
    {
        return;
    }

    printReverseOrderFrom(node->right.get(), out);

    out << node->data->name() << ' '
        << node->data->department() << ' '
        << node->data->salary() << '\n';

    printReverseOrderFrom(node->left.get(), out);
}

Selection EmployeeTree::select(const Criterion& criterion) const
{
    Selection result;

    selectFrom(root_.get(), criterion, result);

    return result;
}

void EmployeeTree::selectFrom(const Node* node,
                               const Criterion& criterion,
                               Selection& result) const
{
    if (node == nullptr)
    {
        return;
    }

    selectFrom(node->left.get(), criterion, result);

    if (criterion.matches(*node->data))
    {
        result.pushBack(node->data);
    }

    selectFrom(node->right.get(), criterion, result);
}
