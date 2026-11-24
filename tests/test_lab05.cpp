#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "criterion.h"
#include "employee.h"
#include "employee_tree.h"
#include "selection.h"

namespace
{
class Lab05Test : public testing::Test
{
protected:
    void SetUp() override { ASSERT_EQ(Employee::aliveCount(), 0); }
    void TearDown() override { EXPECT_EQ(Employee::aliveCount(), 0); }
};

void fillTree(EmployeeTree& tree)
{
    tree.insert(Employee{"Anna", "IT", 80000});
    tree.insert(Employee{"Boris", "Sales", 60000});
    tree.insert(Employee{"Vera", "IT", 100000});
    tree.insert(Employee{"Dina", "IT", 70000});
    tree.insert(Employee{"Egor", "HR", 90000});
    tree.insert(Employee{"Fedor", "Sales", 110000});
}

void expectSalaries(const EmployeeTree& tree, const int* expected,
                    std::size_t count)
{
    AllCriterion all;
    const Selection selection = tree.select(all);
    ASSERT_EQ(selection.size(), count);
    EXPECT_EQ(tree.size(), count);
    for (std::size_t i = 0; i < count; ++i)
    {
        auto employee = selection.at(i).lock();
        ASSERT_TRUE(employee);
        EXPECT_EQ(employee->salary(), expected[i]);
        if (i != 0)
        {
            EXPECT_LT(expected[i - 1], employee->salary());
        }
    }
}

TEST_F(Lab05Test, EmployeeComparesOnlySalary)
{
    Employee a{"Anna", "IT", 80};
    Employee b{"Boris", "Sales", 80};
    Employee c{"Vera", "HR", 100};
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a < a);
    EXPECT_TRUE(a < c);
    EXPECT_FALSE(c < a);
}

TEST_F(Lab05Test, EmployeeRejectsNegativeSalaryWithoutChangingCounter)
{
    EXPECT_THROW((Employee{"Anna", "IT", -1}), std::invalid_argument);
    EXPECT_EQ(Employee::aliveCount(), 0);
    Employee zero{"", "", 0};
    EXPECT_EQ(zero.salary(), 0);
}

TEST_F(Lab05Test, EmployeeSpecialMembersCountObjectsNotAssignments)
{
    Employee original{"Anna", "IT", 80};
    Employee copy{original};
    Employee moved{std::move(copy)};
    EXPECT_EQ(Employee::aliveCount(), 3);
    EXPECT_EQ(moved.name(), "Anna");
    copy = original;
    moved = std::move(copy);
    EXPECT_EQ(Employee::aliveCount(), 3);
    EXPECT_EQ(moved.salary(), 80);
}

TEST_F(Lab05Test, EmptyTreeContract)
{
    EmployeeTree tree;
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), 0u);
    EXPECT_TRUE(tree.find(Employee{"", "", 1}).expired());
    EXPECT_FALSE(tree.remove(Employee{"", "", 1}));
    AllCriterion all;
    EXPECT_TRUE(tree.select(all).empty());
    std::ostringstream out;
    tree.printInOrder(out);
    tree.printReverseOrder(out);
    EXPECT_TRUE(out.str().empty());
}

TEST_F(Lab05Test, InsertAndDuplicateKey)
{
    EmployeeTree tree;
    EXPECT_TRUE(tree.insert(Employee{"Anna", "IT", 80}));
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_FALSE(tree.insert(Employee{"Other", "HR", 80}));
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_EQ(Employee::aliveCount(), 1);
    EXPECT_EQ(tree.find(Employee{"", "", 80}).lock()->name(), "Anna");
}

TEST_F(Lab05Test, SearchReturnsStoredRecordAndMissingKeyIsEmpty)
{
    EmployeeTree tree;
    fillTree(tree);
    auto record = tree.find(Employee{"", "", 60000}).lock();
    ASSERT_TRUE(record);
    EXPECT_EQ(record->name(), "Boris");
    EXPECT_EQ(record->department(), "Sales");
    EXPECT_TRUE(tree.find(Employee{"", "", 95000}).expired());
}

TEST_F(Lab05Test, HeightOfLeafAndSeveralLevels)
{
    EmployeeTree tree;
    tree.insert(Employee{"Anna", "IT", 80});
    EXPECT_EQ(tree.height(), 1u);
    tree.insert(Employee{"Boris", "Sales", 60});
    tree.insert(Employee{"Vera", "IT", 100});
    tree.insert(Employee{"Dina", "IT", 70});
    EXPECT_EQ(tree.height(), 3u);
}

TEST_F(Lab05Test, TraversalsHaveOppositeSalaryOrder)
{
    EmployeeTree tree;
    fillTree(tree);
    std::ostringstream forward;
    std::ostringstream backward;
    tree.printInOrder(forward);
    tree.printReverseOrder(backward);
    EXPECT_EQ(forward.str(),
              "Boris Sales 60000\nDina IT 70000\nAnna IT 80000\n"
              "Egor HR 90000\nVera IT 100000\nFedor Sales 110000\n");
    EXPECT_EQ(backward.str(),
              "Fedor Sales 110000\nVera IT 100000\nEgor HR 90000\n"
              "Anna IT 80000\nDina IT 70000\nBoris Sales 60000\n");
}

TEST_F(Lab05Test, RemoveLeafExpiresOldReferenceAndSearch)
{
    EmployeeTree tree;
    fillTree(tree);
    auto old = tree.find(Employee{"", "", 70000});
    EXPECT_TRUE(tree.remove(Employee{"", "", 70000}));
    EXPECT_TRUE(old.expired());
    EXPECT_TRUE(tree.find(Employee{"", "", 70000}).expired());
    const int expected[] = {60000, 80000, 90000, 100000, 110000};
    expectSalaries(tree, expected, 5);
    EXPECT_EQ(Employee::aliveCount(), 5);
}

TEST_F(Lab05Test, RemoveNodeWithRightChild)
{
    EmployeeTree tree;
    fillTree(tree);
    EXPECT_TRUE(tree.remove(Employee{"", "", 60000}));
    const int expected[] = {70000, 80000, 90000, 100000, 110000};
    expectSalaries(tree, expected, 5);
}

TEST_F(Lab05Test, RemoveRootWithLeftChild)
{
    EmployeeTree tree;
    tree.insert(Employee{"Anna", "IT", 80});
    tree.insert(Employee{"Boris", "Sales", 60});
    EXPECT_TRUE(tree.remove(Employee{"", "", 80}));
    const int expected[] = {60};
    expectSalaries(tree, expected, 1);
    EXPECT_EQ(tree.height(), 1u);
}

TEST_F(Lab05Test, RemoveOnlyRootLeavesEmptyTree)
{
    EmployeeTree tree;
    tree.insert(Employee{"Anna", "IT", 80});
    EXPECT_TRUE(tree.remove(Employee{"", "", 80}));
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.height(), 0u);
    EXPECT_EQ(Employee::aliveCount(), 0);
}

TEST_F(Lab05Test, RemoveTwoChildrenPreservesSuccessorIdentity)
{
    EmployeeTree tree;
    fillTree(tree);
    auto removed = tree.find(Employee{"", "", 80000});
    auto successor = tree.find(Employee{"", "", 90000});
    const Employee* original_address = successor.lock().get();
    EXPECT_TRUE(tree.remove(Employee{"", "", 80000}));
    EXPECT_TRUE(removed.expired());
    EXPECT_FALSE(successor.expired());
    EXPECT_EQ(tree.find(Employee{"", "", 90000}).lock().get(), original_address);
    const int expected[] = {60000, 70000, 90000, 100000, 110000};
    expectSalaries(tree, expected, 5);
    EXPECT_EQ(Employee::aliveCount(), 5);
}

TEST_F(Lab05Test, RemoveTwoChildrenSplicesSuccessorsRightChild)
{
    EmployeeTree tree;
    const int keys[] = {80, 60, 100, 90, 95, 110};
    for (int key : keys)
    {
        tree.insert(Employee{"Person", "IT", key});
    }
    EXPECT_TRUE(tree.remove(Employee{"", "", 80}));
    const int expected[] = {60, 90, 95, 100, 110};
    expectSalaries(tree, expected, 5);
}

TEST_F(Lab05Test, RemoveTwoChildrenWithImmediateSuccessor)
{
    EmployeeTree tree;
    tree.insert(Employee{"Anna", "IT", 80});
    tree.insert(Employee{"Boris", "Sales", 60});
    tree.insert(Employee{"Vera", "IT", 100});
    tree.insert(Employee{"Dina", "IT", 110});
    EXPECT_TRUE(tree.remove(Employee{"", "", 80}));
    const int expected[] = {60, 100, 110};
    expectSalaries(tree, expected, 3);
}

TEST_F(Lab05Test, MissingRemovalDoesNotChangeTree)
{
    EmployeeTree tree;
    fillTree(tree);
    EXPECT_FALSE(tree.remove(Employee{"", "", 95000}));
    const int expected[] = {60000, 70000, 80000, 90000, 100000, 110000};
    expectSalaries(tree, expected, 6);
}

TEST_F(Lab05Test, DepartmentSelectionHasCorrectContentAndOrder)
{
    EmployeeTree tree;
    fillTree(tree);
    DepartmentCriterion it{"IT"};
    Selection selected = tree.select(it);
    ASSERT_EQ(selected.size(), 3u);
    const int expected[] = {70000, 80000, 100000};
    for (std::size_t i = 0; i < selected.size(); ++i)
    {
        auto record = selected.at(i).lock();
        ASSERT_TRUE(record);
        EXPECT_EQ(record->salary(), expected[i]);
        EXPECT_EQ(record->department(), "IT");
    }
}

TEST_F(Lab05Test, DifferentCriteriaAndNoMatches)
{
    EmployeeTree tree;
    fillTree(tree);
    DepartmentCriterion it{"IT"};
    AllCriterion all;
    DepartmentCriterion none{"Unknown"};
    DepartmentCriterion different_case{"it"};
    EXPECT_EQ(tree.select(it).size(), 3u);
    EXPECT_EQ(tree.select(all).size(), 6u);
    EXPECT_TRUE(tree.select(none).empty());
    EXPECT_TRUE(tree.select(different_case).empty());
}

TEST_F(Lab05Test, SelectingDoesNotChangeTreeOrCreateEmployees)
{
    EmployeeTree tree;
    fillTree(tree);
    std::ostringstream before;
    tree.printInOrder(before);
    const auto old_height = tree.height();
    const int old_alive = Employee::aliveCount();
    DepartmentCriterion it{"IT"};
    auto selected = tree.select(it);
    std::ostringstream after;
    tree.printInOrder(after);
    EXPECT_EQ(before.str(), after.str());
    EXPECT_EQ(tree.size(), 6u);
    EXPECT_EQ(tree.height(), old_height);
    EXPECT_EQ(Employee::aliveCount(), old_alive);
}

TEST_F(Lab05Test, SelectionKeepsSizeWhenRecordExpires)
{
    EmployeeTree tree;
    fillTree(tree);
    DepartmentCriterion it{"IT"};
    auto selected = tree.select(it);
    EXPECT_TRUE(tree.remove(Employee{"", "", 80000}));
    EXPECT_EQ(selected.size(), 3u);
    EXPECT_TRUE(selected.at(1).expired());
    EXPECT_FALSE(selected.at(0).expired());
    EXPECT_FALSE(selected.at(2).expired());
}

TEST_F(Lab05Test, DestroyingTreeExpiresSelectionAndReleasesAllRecords)
{
    Selection selected;
    {
        EmployeeTree tree;
        fillTree(tree);
        AllCriterion all;
        selected = tree.select(all);
        EXPECT_EQ(Employee::aliveCount(), 6);
    }
    EXPECT_EQ(Employee::aliveCount(), 0);
    ASSERT_EQ(selected.size(), 6u);
    for (std::size_t i = 0; i < selected.size(); ++i)
    {
        EXPECT_TRUE(selected.at(i).expired());
    }
}

TEST_F(Lab05Test, LockTemporarilyExtendsRecordLifetime)
{
    EmployeeTree tree;
    tree.insert(Employee{"Anna", "IT", 80});
    auto observer = tree.find(Employee{"", "", 80});
    auto access = observer.lock();
    EXPECT_TRUE(tree.remove(Employee{"", "", 80}));
    EXPECT_EQ(Employee::aliveCount(), 1);
    EXPECT_FALSE(observer.expired());
    access.reset();
    EXPECT_EQ(Employee::aliveCount(), 0);
    EXPECT_TRUE(observer.expired());
}

TEST_F(Lab05Test, EmptySelectionAndBounds)
{
    Selection selection;
    EXPECT_TRUE(selection.empty());
    EXPECT_EQ(selection.size(), 0u);
    EXPECT_EQ(selection.capacity(), 0u);
    EXPECT_THROW(selection.at(0), std::out_of_range);
    selection.pushBack({});
    EXPECT_FALSE(selection.empty());
    EXPECT_TRUE(selection.at(0).expired());
    EXPECT_THROW(selection.at(1), std::out_of_range);
}

TEST_F(Lab05Test, SelectionCapacityDoublesAndKeepsElements)
{
    EmployeeTree tree;
    tree.insert(Employee{"Anna", "IT", 80});
    auto observer = tree.find(Employee{"", "", 80});
    Selection selected;
    const std::size_t expected_capacity[] = {1, 2, 4, 4, 8};
    for (std::size_t i = 0; i < 5; ++i)
    {
        selected.pushBack(observer);
        EXPECT_EQ(selected.capacity(), expected_capacity[i]);
        EXPECT_EQ(selected.size(), i + 1);
    }
    for (std::size_t i = 0; i < selected.size(); ++i)
    {
        ASSERT_FALSE(selected.at(i).expired());
        EXPECT_EQ(selected.at(i).lock()->salary(), 80);
    }
    EXPECT_EQ(observer.use_count(), 1);
}

TEST_F(Lab05Test, SelectionCopyHasIndependentArrayAndSharedObservations)
{
    EmployeeTree tree;
    fillTree(tree);
    DepartmentCriterion it{"IT"};
    auto original = tree.select(it);
    Selection copy{original};
    original.pushBack(tree.find(Employee{"", "", 60000}));
    EXPECT_EQ(original.size(), 4u);
    EXPECT_EQ(copy.size(), 3u);
    EXPECT_TRUE(tree.remove(Employee{"", "", 80000}));
    EXPECT_TRUE(original.at(1).expired());
    EXPECT_TRUE(copy.at(1).expired());
    EXPECT_EQ(Employee::aliveCount(), 5);
}

TEST_F(Lab05Test, SelectionCopyAssignmentReplacesArrayAndHandlesSelfAssignment)
{
    EmployeeTree tree;
    fillTree(tree);
    AllCriterion all;
    auto original = tree.select(all);
    Selection target;
    target.pushBack({});
    target = original;
    original.pushBack({});
    EXPECT_EQ(target.size(), 6u);
    Selection& alias = target;
    target = alias;
    EXPECT_EQ(target.size(), 6u);
    EXPECT_EQ(target.at(0).lock()->salary(), 60000);
    target = Selection{};
    EXPECT_TRUE(target.empty());
    EXPECT_EQ(Employee::aliveCount(), 6);
}

TEST_F(Lab05Test, SelectionMoveConstructorEmptiesAndAllowsReuseOfSource)
{
    EmployeeTree tree;
    fillTree(tree);
    AllCriterion all;
    auto source = tree.select(all);
    const auto capacity = source.capacity();
    Selection target{std::move(source)};
    EXPECT_EQ(target.size(), 6u);
    EXPECT_EQ(target.capacity(), capacity);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.capacity(), 0u);
    source.pushBack({});
    EXPECT_EQ(source.size(), 1u);
    EXPECT_EQ(target.at(0).lock()->salary(), 60000);
}

TEST_F(Lab05Test, SelectionMoveAssignmentAndSelfMove)
{
    EmployeeTree tree;
    fillTree(tree);
    AllCriterion all;
    auto source = tree.select(all);
    Selection target;
    target.pushBack({});
    target = std::move(source);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.capacity(), 0u);
    EXPECT_EQ(target.size(), 6u);
    Selection& alias = target;
    target = std::move(alias);
    EXPECT_EQ(target.size(), 6u);
    EXPECT_EQ(target.at(0).lock()->salary(), 60000);
}

TEST_F(Lab05Test, EmptyAndExpiredSelectionsCanBeCopiedAndMoved)
{
    Selection empty;
    Selection copy{empty};
    EXPECT_TRUE(copy.empty());
    empty.pushBack({});
    Selection expired{empty};
    Selection moved{std::move(expired)};
    EXPECT_EQ(moved.size(), 1u);
    EXPECT_TRUE(moved.at(0).expired());
    EXPECT_TRUE(expired.empty());
}

TEST_F(Lab05Test, TreeMoveConstructorPreservesRecordsAndAllowsSourceReuse)
{
    EmployeeTree source;
    fillTree(source);
    auto observer = source.find(Employee{"", "", 80000});
    EmployeeTree target{std::move(source)};
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.height(), 0u);
    EXPECT_EQ(target.size(), 6u);
    EXPECT_FALSE(observer.expired());
    EXPECT_EQ(Employee::aliveCount(), 6);
    source.insert(Employee{"New", "HR", 1});
    EXPECT_EQ(source.size(), 1u);
    EXPECT_EQ(target.size(), 6u);
}

TEST_F(Lab05Test, TreeMoveAssignmentReleasesOldTreeAndHandlesSelfMove)
{
    EmployeeTree source;
    fillTree(source);
    EmployeeTree target;
    target.insert(Employee{"Old", "HR", 1});
    auto old = target.find(Employee{"", "", 1});
    target = std::move(source);
    EXPECT_TRUE(old.expired());
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.height(), 0u);
    EXPECT_EQ(target.size(), 6u);
    EXPECT_EQ(Employee::aliveCount(), 6);
    EmployeeTree& alias = target;
    target = std::move(alias);
    EXPECT_EQ(target.size(), 6u);
}

TEST_F(Lab05Test, TypePoliciesPreventTreeCopiesAndKeyMutation)
{
    EXPECT_FALSE(std::is_copy_constructible_v<EmployeeTree>);
    EXPECT_FALSE(std::is_copy_assignable_v<EmployeeTree>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<EmployeeTree>);
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<EmployeeTree>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<Selection>);
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<Selection>);
    EXPECT_TRUE(std::is_abstract_v<Criterion>);
    EXPECT_TRUE(std::has_virtual_destructor_v<Criterion>);
    EmployeeTree tree;
    EXPECT_TRUE((std::is_same_v<decltype(tree.find(Employee{"", "", 0})),
                                std::weak_ptr<const Employee>>));
}

TEST_F(Lab05Test, EveryInsertionAndRemovalMaintainsSortedUniqueKeys)
{
    EmployeeTree tree;
    bool present[31] = {};
    int expected[31] = {};
    std::size_t count = 0;
    for (int i = 0; i < 31; ++i)
    {
        const int key = (i * 17) % 31;
        ASSERT_TRUE(tree.insert(Employee{"Person", "IT", key}));
        present[key] = true;
        count = 0;
        for (int value = 0; value < 31; ++value)
        {
            if (present[value]) expected[count++] = value;
        }
        expectSalaries(tree, expected, count);
    }
    for (int i = 0; i < 31; ++i)
    {
        const int key = (i * 13) % 31;
        ASSERT_TRUE(tree.remove(Employee{"", "", key}));
        present[key] = false;
        count = 0;
        for (int value = 0; value < 31; ++value)
        {
            if (present[value]) expected[count++] = value;
        }
        expectSalaries(tree, expected, count);
        EXPECT_EQ(Employee::aliveCount(), static_cast<int>(count));
    }
    EXPECT_EQ(tree.height(), 0u);
}
}
