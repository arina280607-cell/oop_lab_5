#include <iostream>
#include <stdexcept>

#include "criterion.h"
#include "employee.h"
#include "employee_tree.h"
#include "selection.h"

namespace
{
void printSelection(const Selection& selection, std::ostream& out)
{
    out << selection.size() << '\n';
    for (std::size_t i = 0; i < selection.size(); ++i)
    {
        auto employee = selection.at(i).lock();
        if (employee)
        {
            out << employee->name() << ' '
                << employee->department() << ' '
                << employee->salary() << '\n';
        }
        else
        {
            out << "[expired]\n";
        }
    }
}
}

int main()
{
    try
    {
        {
            EmployeeTree tree;
            tree.insert(Employee{"Anna", "IT", 80000});
            tree.insert(Employee{"Boris", "Sales", 60000});
            tree.insert(Employee{"Vera", "IT", 100000});
            tree.insert(Employee{"Dina", "IT", 70000});
            tree.insert(Employee{"Egor", "HR", 90000});
            tree.insert(Employee{"Fedor", "Sales", 110000});

            std::cout << "Variant 2: employees\n";
            std::cout << "In-order traversal:\n";
            tree.printInOrder(std::cout);
            std::cout << "Size: " << tree.size()
                      << ", height: " << tree.height() << '\n';

            DepartmentCriterion criterion{"IT"};
            Selection selected = tree.select(criterion);

            std::cout << "Selection: department IT\n";
            printSelection(selected, std::cout);

            // Anna is the root and has two children.
            if (!tree.remove(Employee{"", "", 80000}))
            {
                throw std::logic_error("Demonstration record not found");
            }

            std::cout << "After removing salary 80000 (same selection):\n";
            printSelection(selected, std::cout);
            std::cout << "Tree after removal:\n";
            tree.printInOrder(std::cout);
        }

        std::cout << "Alive after tree destruction: "
                  << Employee::aliveCount() << '\n';
        return Employee::aliveCount() == 0 ? 0 : 1;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
