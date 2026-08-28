#include <Core/Containers/List.h>
#include <Core/Containers/Map.h>
#include <Core/Containers/SparseSet.h>
#include <Core/Containers/SparseArray.h>
#include <Core/ManiTypes.h>

#include <ManiZ/ManiZ.h>
#include <ManiMaths/Vec3.h>
#include <ManiTests/ManiTests.h>


struct MoveTester
{
    static inline int copies = 0;
    static inline int moves = 0;

    int value;

    MoveTester(int v = 0) : value(v) {}

    // copy
    MoveTester(const MoveTester& other) : value(other.value) { ++copies; }
    MoveTester& operator=(const MoveTester& other) { value = other.value; ++copies; return *this; }

    // move
    MoveTester(MoveTester&& other) noexcept : value(other.value) { other.value = -1; ++moves; }
    MoveTester& operator=(MoveTester&& other) noexcept { value = other.value; other.value = -1; ++moves; return *this; }

    static void reset() { copies = moves = 0; }
};

MANI_SECTION_BEGIN(List, "Containers")
{
    MANI_TEST(ListOperations, "List operations")
    {
        {
            // constructors
            Mani::List<int> empty;
            MANI_TEST_ASSERT(empty.count() == 0, "Empty list should start with count 0");

            Mani::List<int> filled(5, 42);
            MANI_TEST_ASSERT(filled.count() == 5, "Filled list should have 5 elements");
            MANI_TEST_ASSERT(filled[0] == 42, "First element should be 42");
            MANI_TEST_ASSERT(filled.last() == 42, "Last element should be 42");

            Mani::List<int> init{ 1, 2, 3, 4, 5 };
            MANI_TEST_ASSERT(init.count() == 5, "Initializer list should have 5 elements");
            MANI_TEST_ASSERT(init.first() == 1, "First element should be 1");
            MANI_TEST_ASSERT(init.last() == 5, "Last element should be 5");
        }

        {
            // add / pop
            Mani::List<int> l;
            l.add(10);
            l.add(20);
            MANI_TEST_ASSERT(l.count() == 2, "List should contain 2 elements after add()");
            MANI_TEST_ASSERT(l[0] == 10, "First element should be 10");
            MANI_TEST_ASSERT(l.pop() == 20, "Pop should return the last element (20)");
            MANI_TEST_ASSERT(l.count() == 1, "List should contain 1 element after pop()");
        }

        {
            // enqueue / dequeue
            Mani::List<int> l;
            l.enqueue(30);
            l.enqueue(40);
            MANI_TEST_ASSERT(l.first() == 40, "First element should be 40 after enqueue");
            MANI_TEST_ASSERT(l.dequeue() == 40, "Dequeue should return 40 first");
            MANI_TEST_ASSERT(l.dequeue() == 30, "Dequeue should then return 30");
            MANI_TEST_ASSERT(l.count() == 0, "List should be empty after two dequeues");
        }

        {
            // add / remove
            Mani::List<int> l{ 1, 2, 3 };
            MANI_TEST_ASSERT(l.remove(2) == true, "Should have removed 2");
            MANI_TEST_ASSERT(l.count() == 2, "Should have 2 elements");
            MANI_TEST_ASSERT(l.remove(10) == false, "Should not have removed anything");
            MANI_TEST_ASSERT(l.count() == 2, "Should still have 2 elements");
        }

        {
            // contains / indexOf / addUnique
            Mani::List<int> l{ 1, 2, 3 };
            MANI_TEST_ASSERT(l.contains(2), "List should contain 2");
            MANI_TEST_ASSERT(l.indexOf(3) == 2, "Index of 3 should be 2");
            l.addUnique(3);
            MANI_TEST_ASSERT(l.count() == 3, "addUnique(3) should not increase count");
            l.addUnique(4);
            MANI_TEST_ASSERT(l.contains(4), "List should contain 4 after addUnique(4)");
        }

        {
            // removeIf
            Mani::List<int> l{ 1, 2, 3, 4, 5 };
            const bool didRemove = l.removeIf([](int v) { return v % 2 == 0; });
            MANI_TEST_ASSERT(didRemove == true, "Should have removed even numbers")
            MANI_TEST_ASSERT(l.count() == 3, "List should contain only odd numbers");
            MANI_TEST_ASSERT(l[1] == 3, "Second element should be 3 after removing evens");
        }

        {
            // removeAt
            Mani::List<int>l{ 1, 2, 3, 4, 5 };
            l.removeAt(2);
            MANI_TEST_ASSERT(l.count() == 4, "should have 4 members");
            MANI_TEST_ASSERT(l[2] == 4, "4 should be at index 2 now");
        }

        {
            // atOrDefault
            Mani::List<int> l{ 10, 20 };
            MANI_TEST_ASSERT(l.at(0) == 10, "at(0) should return 10");
            MANI_TEST_ASSERT(l.atPtr(5) == nullptr, "Out of bounds atPtr should return nullptr because it was not added");
        }

        {
            // iterate
            Mani::List<int> l{ 1, 2, 3, 4 };

            {
                int acc = 0;
                for (const auto& i : l)
                {
                    acc += i;
                }
                MANI_TEST_ASSERT(acc == 10, "Accumulation should equal 10");
            }

            for (auto& i : l)
            {
                i += 1;
            }

            {
                int acc = 0;
                for (const auto& i : l)
                {
                    acc += i;
                }
                MANI_TEST_ASSERT(acc == 14, "Accumulation should equal 14 after mutation");
            }
        }
    }

    MANI_TEST(ListRemoveSwapAt, "Should remove elements with swap")
    {
        Mani::List<int> list = { 10, 20, 30, 40 };

        // --- Remove by index ---
        bool removed = list.removeSwapAt(1); // remove element at index 1 (20)
        MANI_TEST_ASSERT(removed, "Should succeed removing index 1");
        MANI_TEST_ASSERT(list.count() == 3, "List should shrink to 3 elements");
        MANI_TEST_ASSERT(list.contains(20) == false, "Value 20 should be gone");
        MANI_TEST_ASSERT(list.contains(40) == true, "Value 40 should be swapped into index 1");

        // --- Remove by value ---
        removed = list.removeSwap(30); // remove element with value 30
        MANI_TEST_ASSERT(removed, "Should succeed removing value 30");
        MANI_TEST_ASSERT(list.count() == 2, "List should shrink to 2 elements");
        MANI_TEST_ASSERT(list.contains(30) == false, "Value 30 should be gone");

        // --- Remove non-existing value ---
        removed = list.removeSwap(99);
        MANI_TEST_ASSERT(removed == false, "Should fail to remove value not in list");
        MANI_TEST_ASSERT(list.count() == 2, "List should remain unchanged");

        // --- Remove invalid index ---
        removed = list.removeSwapAt(100);
        MANI_TEST_ASSERT(removed == false, "Should fail to remove invalid index");
        MANI_TEST_ASSERT(list.count() == 2, "List should remain unchanged after invalid index");
    }

    MANI_SECTION_BEGIN(MoveSemantics, "move semantics")
    {
        MANI_TEST(AddCopyList, "Add with copy semantics")
        {
            MoveTester::reset();

            Mani::List<MoveTester> l;
            MoveTester mt(123);

            l.add(mt); // should copy
            MANI_TEST_ASSERT(l.count() == 1, "List should have 1 element after add(copy)");
            MANI_TEST_ASSERT(l[0].value == 123, "Element should contain copied value 123");
            MANI_TEST_ASSERT(mt.value == 123, "Original should remain unchanged after copy");
            MANI_TEST_ASSERT(MoveTester::copies > 0, "Copy constructor should be used");
            MANI_TEST_ASSERT(MoveTester::moves == 0, "Move constructor should not be called");
        }

        MANI_TEST(AddMoveList, "Add and Move semantics")
        {
            MoveTester::reset();

            Mani::List<MoveTester> l;
            MoveTester mt(42);

            l.add(std::move(mt)); // should trigger move ctor
            MANI_TEST_ASSERT(l.count() == 1, "List should have 1 element after add(move)");
            MANI_TEST_ASSERT(l[0].value == 42, "Element should contain moved value 42");
            MANI_TEST_ASSERT(mt.value == -1, "Moved-from object should be marked -1");
            MANI_TEST_ASSERT(MoveTester::moves > 0, "Move constructor should be used");
            MANI_TEST_ASSERT(MoveTester::copies == 0, "Copy constructor should not be called");
        }

        MANI_TEST(EnqueueMoveList, "Enqueue and Move semantics")
        {
            MoveTester::reset();

            Mani::List<MoveTester> l;
            l.enqueue(MoveTester(99)); // temporary rvalue, should move

            MANI_TEST_ASSERT(l.count() == 1, "List should have 1 element after enqueue(move)");
            MANI_TEST_ASSERT(l.first().value == 99, "First element should be 99");
            MANI_TEST_ASSERT(MoveTester::moves > 0, "Move constructor should be used");
            MANI_TEST_ASSERT(MoveTester::copies == 0, "Copy constructor should not be called");
        }
    }
    MANI_SECTION_END(MoveSemantics)

    MANI_SECTION_BEGIN(Insert, "Containers - Insert semantics")
    {
        MANI_TEST(InsertCopy, "Insert with copy semantics")
        {
            MoveTester::reset();

            Mani::List<MoveTester> l;
            l.add(MoveTester(1)); // move
            l.add(MoveTester(3)); // move

            MoveTester mt(2);
            l.insert(1, mt); // should copy

            MANI_TEST_ASSERT(l.count() == 3, "List should contain 3 elements after insert(copy)");
            MANI_TEST_ASSERT(l[0].value == 1, "First element should be 1");
            MANI_TEST_ASSERT(l[1].value == 2, "Inserted element should be 2 (copied)");
            MANI_TEST_ASSERT(l[2].value == 3, "Last element should be 3");
            MANI_TEST_ASSERT(mt.value == 2, "Original value should remain unchanged after copy insert");
            MANI_TEST_ASSERT(MoveTester::copies > 0, "Copy constructor should have been called");
        }

        MANI_TEST(InsertMove, "Insert with move semantics")
        {
            MoveTester::reset();

            Mani::List<MoveTester> l;
            l.add(MoveTester(1)); // move
            l.add(MoveTester(3)); // move

            MoveTester mt(2);
            l.insert(1, std::move(mt)); // should move

            MANI_TEST_ASSERT(l.count() == 3, "List should contain 3 elements after insert(move)");
            MANI_TEST_ASSERT(l[0].value == 1, "First element should be 1");
            MANI_TEST_ASSERT(l[1].value == 2, "Inserted element should be 2 (moved)");
            MANI_TEST_ASSERT(l[2].value == 3, "Last element should be 3");
            MANI_TEST_ASSERT(mt.value == -1, "Moved-from object should be marked -1");
            MANI_TEST_ASSERT(MoveTester::moves > 0, "Move constructor should have been called");
        }

        MANI_TEST(InsertRValue, "Insert temporary rvalue")
        {
            MoveTester::reset();

            Mani::List<MoveTester> l;
            l.add(MoveTester(10));
            l.insert(1, MoveTester(20)); // directly insert rvalue

            MANI_TEST_ASSERT(l.count() == 2, "List should contain 2 elements after insert(rvalue)");
            MANI_TEST_ASSERT(l[0].value == 10, "First element should be 10");
            MANI_TEST_ASSERT(l[1].value == 20, "Inserted element should be 20");
            MANI_TEST_ASSERT(MoveTester::moves > 0, "Move constructor should be used for temporary insert");
            MANI_TEST_ASSERT(MoveTester::copies == 0, "Copy constructor should not be called");
        }

        MANI_TEST(ListAppend, "Should append elements from another list (copy)")
        {
            Mani::List<int> a = { 1, 2, 3 };
            Mani::List<int> b = { 4, 5, 6 };

            a.append(b);

            // a should contain both
            MANI_TEST_ASSERT(a.count() == 6, "a should contain 6 elements after append");
            MANI_TEST_ASSERT(a[0] == 1 && a[5] == 6, "elements should be in correct order");

            // b should remain intact since it was copied
            MANI_TEST_ASSERT(b.count() == 3, "b should remain unchanged after copy-append");
            MANI_TEST_ASSERT(b[0] == 4 && b[2] == 6, "b should still contain its original elements");
        }

        MANI_TEST(List_FindIf, "Should find elements with predicate")
        {
            Mani::List<int> list = { 1, 2, 3, 4, 5 };

            // --- find first even number ---
            int* found = list.findIf([](int v) { return v % 2 == 0; });
            MANI_TEST_ASSERT(found != nullptr, "Should find an even number");
            MANI_TEST_ASSERT(*found == 2, "First even number should be 2");

            // --- find first greater than 3 ---
            found = list.findIf([](int v) { return v > 3; });
            MANI_TEST_ASSERT(found != nullptr, "Should find a number greater than 3");
            MANI_TEST_ASSERT(*found == 4, "First number greater than 3 should be 4");

            // --- try to find something not in the list ---
            found = list.findIf([](int v) { return v > 10; });
            MANI_TEST_ASSERT(found == nullptr, "Should return nullptr if no element matches");

            // --- const version ---
            const Mani::List<int>& constList = list;
            const int* cfound = constList.findIf([](int v) { return v % 2 == 1; });
            MANI_TEST_ASSERT(cfound != nullptr, "Const findIf should work too");
            MANI_TEST_ASSERT(*cfound == 1, "First odd number should be 1");
        }
    }
    MANI_SECTION_END(Insert)

    MANI_TEST(Serialize, "Should serialize list")
    {
        struct Test
        {
            Mani::List<Mani::Vec3i> data;
        };

        Test test = {
            .data = {
                { 0, 1, 2 },
                { 3, 4, 5 },
                { 6, 7, 8 }
            }
        };
        auto json = ManiZ::to::json(test);
        auto result = ManiZ::from::json<Test>(json);

        MANI_TEST_ASSERT(test.data == result.data, "Should be able to serialize and deserialize a list");
    }

    MANI_SECTION_BEGIN(ListSort, "List - Sort Functions")
    {
        MANI_TEST(DefaultSort, "Should sort list using default < operator")
        {
            Mani::List<int> l{ 5, 2, 8, 1, 3 };
            l.sort();
            MANI_TEST_ASSERT(l[0] == 1, "First element should be 1 after sort");
            MANI_TEST_ASSERT(l[4] == 8, "Last element should be 8 after sort");

            const Mani::List<int> copy = l;
            Mani::List<int> copySorted = copy.sortCopy(); // const version that returns copy
            MANI_TEST_ASSERT(copySorted == l, "Copy returned from const sort() should match sorted list");
        }

        MANI_TEST(PredicateSort, "Should sort list using custom predicate")
        {
            Mani::List<int> l{ 5, 2, 8, 1, 3 };
            l.sort([](const int& a, const int& b) { return a > b; }); // descending
            MANI_TEST_ASSERT(l[0] == 8, "First element should be 8 after descending sort");
            MANI_TEST_ASSERT(l[4] == 1, "Last element should be 1 after descending sort");
            
            const Mani::List<int> copy = l;
            Mani::List<int> copySorted = copy.sortCopy([](const int& a, const int& b) { return a < b; }); // ascending copy
            MANI_TEST_ASSERT(copySorted[0] == 1 && copySorted[4] == 8, "Copy sort should be ascending");
        }

        MANI_TEST(SortAlreadySorted, "Sorting an already sorted list should not change order")
        {
            Mani::List<int> l{ 1, 2, 3, 4, 5 };
            l.sort();
            MANI_TEST_ASSERT(l[0] == 1 && l[4] == 5, "List should remain sorted");
        }

        MANI_TEST(SortEmptyList, "Sorting an empty list should not crash")
        {
            Mani::List<int> l;
            l.sort();
            MANI_TEST_ASSERT(l.count() == 0, "Empty list should remain empty after sort");
        }
    }
    MANI_SECTION_END(ListSort)
}
MANI_SECTION_END(List)

MANI_SECTION_BEGIN(Map, "Containers")
{
    MANI_TEST(MapConstructors, "Constructors")
    {
        {
            Mani::Map<int, std::string> empty;
            MANI_TEST_ASSERT(empty.count() == 0, "Empty map should start with count 0");
            MANI_TEST_ASSERT(empty.isEmpty(), "Empty map should be marked empty");
        }

        {
            Mani::Map<int, std::string> init{
                {1, "one"},
                {2, "two"},
                {3, "three"}
            };

            MANI_TEST_ASSERT(init.count() == 3, "Initializer list map should have 3 elements");
            MANI_TEST_ASSERT(init.has(1), "Map should contain key 1");
            MANI_TEST_ASSERT(init.get(2) == "two", "Key 2 should map to 'two'");
        }
    }

    MANI_TEST(MapAddAndAccess, "Add and operator[]")
    {
        Mani::Map<std::string, int> m;
        m.add("apple", 5);
        m.add("banana", 10);

        MANI_TEST_ASSERT(m.count() == 2, "Map should contain 2 elements after add()");
        MANI_TEST_ASSERT(m.get("apple") == 5, "apple should map to 5");
        MANI_TEST_ASSERT(m.get("banana") == 10, "banana should map to 10");

        // operator[] inserts if not present
        m["cherry"] = 15;
        MANI_TEST_ASSERT(m.count() == 3, "Operator[] should insert new key if missing");
        MANI_TEST_ASSERT(m["cherry"] == 15, "cherry should map to 15");

        // operator[] modifies if present
        m["apple"] = 99;
        MANI_TEST_ASSERT(m.get("apple") == 99, "apple should now map to 99");
    }

    MANI_TEST(MapRemove, "Remove elements")
    {
        Mani::Map<int, std::string> m;
        m.add(1, "a");
        m.add(2, "b");
        m.add(3, "c");

        bool removed = m.remove(2);
        MANI_TEST_ASSERT(removed, "remove(2) should succeed");
        MANI_TEST_ASSERT(m.count() == 2, "Map should contain 2 elements after remove()");
        MANI_TEST_ASSERT(!m.has(2), "Key 2 should no longer exist");

        removed = m.remove(42);
        MANI_TEST_ASSERT(!removed, "Removing a non-existent key should return false");
    }

    MANI_TEST(MapHasAndGet, "Has and Get")
    {
        Mani::Map<std::string, int> m{
            {"alpha", 1},
            {"beta", 2}
        };

        MANI_TEST_ASSERT(m.has("alpha"), "Map should contain key 'alpha'");
        MANI_TEST_ASSERT(!m.has("gamma"), "Map should not contain key 'gamma'");
        MANI_TEST_ASSERT(m.get("beta") == 2, "beta should map to 2");
    }

    MANI_TEST(MapIterators, "Iterators")
    {
        Mani::Map<int, std::string> m{
            {1, "one"},
            {2, "two"},
            {3, "three"}
        };

        int keySum = 0;
        std::string concatenated;

        for (auto kv : m)
        {
            keySum += kv.key;
            concatenated += kv.value;
        }

        MANI_TEST_ASSERT(keySum == 6, "Sum of keys (1+2+3) should equal 6");
        MANI_TEST_ASSERT(concatenated.find("one") != std::string::npos, "Concatenated string should contain 'one'");
        MANI_TEST_ASSERT(concatenated.find("two") != std::string::npos, "Concatenated string should contain 'two'");
        MANI_TEST_ASSERT(concatenated.find("three") != std::string::npos, "Concatenated string should contain 'three'");

        // Const iterator
        const auto& cm = m;
        int count = 0;
        for (auto kv : cm)
        {
            ++count;
            MANI_TEST_ASSERT(m.has(kv.key), "Const iterator should return valid key");
        }
        MANI_TEST_ASSERT(count == 3, "Const iterator should visit all 3 elements");
    }

    MANI_TEST(MapFind, "Should return optional reference to value if key exists")
    {
        Mani::Map<int, std::string> map;
        map.add(1, "one");
        map.add(2, "two");

        // key exists
        auto* opt1 = map.find(1);
        MANI_TEST_ASSERT(opt1 != nullptr, "Should return a value for an existing key");
        MANI_TEST_ASSERT(*opt1 == "one", "Value for key 1 should be 'one'");

        auto* opt2 = map.find(2);
        MANI_TEST_ASSERT(opt2 != nullptr, "Should return a value for an existing key");
        MANI_TEST_ASSERT(*opt2 == "two", "Value for key 2 should be 'two'");

        // key does not exist
        auto* opt3 = map.find(3);
        MANI_TEST_ASSERT(opt3 == nullptr, "Should return empty optional for a missing key");
    }

    MANI_TEST(Map, "Iterate and mutate values by reference")
    {
        Mani::Map<int, int> map;
        map[1] = 10;
        map[2] = 20;
        map[3] = 30;

        // Iterate by reference and mutate values
        for (auto& [k, v] : map)
        {
            v += 5; // allowed (value is mutable)
            // k = 42; // should not compile (key is const)
        }

        // Verify values updated
        MANI_TEST_ASSERT(map[1] == 15, "Value for key 1 should be updated");
        MANI_TEST_ASSERT(map[2] == 25, "Value for key 2 should be updated");
        MANI_TEST_ASSERT(map[3] == 35, "Value for key 3 should be updated");

        // Also test const iteration
        const auto& cmap = map;
        for (const auto& [k, v] : cmap)
        {
            MANI_TEST_ASSERT(v % 5 == 0, "Const iteration should see updated multiples of 5");
        }
    }
}
MANI_SECTION_END(Map)

MANI_SECTION_BEGIN(CopyMoveOperators, "Copy and Move Operators")
{
    MANI_SECTION_BEGIN(ListOperators, "List Copy and Move")
    {
        MANI_TEST(CopyConstructorList, "Copy constructor should duplicate the list")
        {
            Mani::List<int> original{ 1, 2, 3 };
            Mani::List<int> copy(original);

            MANI_TEST_ASSERT(copy.count() == original.count(), "Copy should have same count as original");
            MANI_TEST_ASSERT(copy == original, "Copy should equal original");
        }

        MANI_TEST(CopyAssignmentList, "Copy assignment should duplicate the list")
        {
            Mani::List<int> original{ 4, 5, 6 };
            Mani::List<int> copy;
            copy = original;

            MANI_TEST_ASSERT(copy.count() == original.count(), "Copy should have same count as original");
            MANI_TEST_ASSERT(copy == original, "Copy should equal original");
        }

        MANI_TEST(MoveConstructorList, "Move constructor should transfer ownership")
        {
            Mani::List<int> original{ 7, 8, 9 };
            Mani::List<int> moved(std::move(original));

            MANI_TEST_ASSERT(moved.count() == 3, "Moved list should have 3 elements");
            MANI_TEST_ASSERT(original.count() == 0, "Original list should be empty after move");
        }

        MANI_TEST(MoveAssignmentList, "Move assignment should transfer ownership")
        {
            Mani::List<int> original{ 10, 11, 12 };
            Mani::List<int> moved;
            moved = std::move(original);

            MANI_TEST_ASSERT(moved.count() == 3, "Moved list should have 3 elements");
            MANI_TEST_ASSERT(original.count() == 0, "Original list should be empty after move");
        }
    }
    MANI_SECTION_END(ListOperators)

    MANI_SECTION_BEGIN(MapOperators, "Map Copy and Move")
    {
        MANI_TEST(CopyConstructorMap, "Copy constructor should duplicate the map")
        {
            Mani::Map<int, int> original;
            original.add(1, 100);
            original.add(2, 200);

            Mani::Map<int, int> copy(original);

            MANI_TEST_ASSERT(copy.count() == original.count(), "Copy should have same count as original");
            MANI_TEST_ASSERT(copy[1] == 100 && copy[2] == 200, "Copy should preserve key/value pairs");
        }

        MANI_TEST(CopyAssignmentMap, "Copy assignment should duplicate the map")
        {
            Mani::Map<int, int> original;
            original.add(3, 300);
            original.add(4, 400);

            Mani::Map<int, int> copy;
            copy = original;

            MANI_TEST_ASSERT(copy.count() == original.count(), "Copy should have same count as original");
            MANI_TEST_ASSERT(copy[3] == 300 && copy[4] == 400, "Copy should preserve key/value pairs");
        }

        MANI_TEST(MoveConstructorMap, "Move constructor should transfer ownership")
        {
            Mani::Map<int, int> original;
            original.add(5, 500);
            original.add(6, 600);

            Mani::Map<int, int> moved(std::move(original));

            MANI_TEST_ASSERT(moved.count() == 2, "Moved map should have 2 elements");
            MANI_TEST_ASSERT(original.count() == 0, "Original map should be empty after move");
        }

        MANI_TEST(MoveAssignmentMap, "Move assignment should transfer ownership")
        {
            Mani::Map<int, int> original;
            original.add(7, 700);
            original.add(8, 800);

            Mani::Map<int, int> moved;
            moved = std::move(original);

            MANI_TEST_ASSERT(moved.count() == 2, "Moved map should have 2 elements");
            MANI_TEST_ASSERT(original.count() == 0, "Original map should be empty after move");
        }

        MANI_TEST(MapGetOrAdd, "getOrAdd should insert or return existing value")
        {
            Mani::Map<int, std::string> map;

            // Case 1: Key does not exist, should insert
            {
                std::string expected = "hello";
                std::string& ref = map.getOrAdd(1, expected);

                MANI_TEST_ASSERT(map.count() == 1, "Map should have one element after insertion");
                MANI_TEST_ASSERT(map[1] == "hello", "Inserted value should match");
                MANI_TEST_ASSERT(&ref == &map[1], "Returned reference should point to stored value");
            }

            // Case 2: Key already exists, should not insert again
            {
                std::string& ref = map.getOrAdd(1, "world");

                MANI_TEST_ASSERT(map.count() == 1, "Map size should not increase if key exists");
                MANI_TEST_ASSERT(map[1] == "hello", "Existing value should not be overwritten");
                MANI_TEST_ASSERT(&ref == &map[1], "Returned reference should point to existing value");
            }

            // Case 3: Insert a different key
            {
                std::string& ref = map.getOrAdd(2, "new");

                MANI_TEST_ASSERT(map.count() == 2, "Second key should be inserted");
                MANI_TEST_ASSERT(map[2] == "new", "New value should be stored correctly");
                MANI_TEST_ASSERT(&ref == &map[2], "Returned reference should point to new value");
            }
        }
    }
    MANI_SECTION_END(MapOperators)

    MANI_SECTION_BEGIN(SparseSet, "SparseSet operations")
    {
        MANI_TEST(SparseSetBasic, "Should be able to construct and use the basic functionalities of a sparse set")
        {
            struct MyComponent
            {
                int x = 1;
                int y = 1;
            };
            
            Mani::SparseSet<MyComponent> sparseSet;
            
            {
                sparseSet.insert(3, MyComponent{5, 10});
                MANI_TEST_ASSERT(sparseSet.count() == 1, "One element was added");
            }
            {
                MyComponent& component = sparseSet.get(3);
                MANI_TEST_ASSERT(component.x == 5, "Should be equal to new value");
                MANI_TEST_ASSERT(component.y == 10, "Should be equal to new value");
            }
            {
                MyComponent* ptr = sparseSet.getPtr(5);
                MANI_TEST_ASSERT(ptr == nullptr, "5 wasn't created yet.");
            }
            {
                for (MyComponent& component : sparseSet.getDense())
                {
                    MANI_TEST_ASSERT(component.x == 5, "Should be equal to new value");
                    MANI_TEST_ASSERT(component.y == 10, "Should be equal to new value");
                }
            }
            {
                const Mani::SizeT result = sparseSet.removeSwap(3);
                MANI_TEST_ASSERT(result == Mani::INDEX_NONE, "should have removed element");
                MANI_TEST_ASSERT(sparseSet.isEmpty(), "should be empty");
            }
        }

        MANI_TEST(SparseSetMultipleInsert, "Should handle multiple non-contiguous inserts")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(10, 100);
            sparseSet.insert(42, 420);
            sparseSet.insert(7, 70);

            MANI_TEST_ASSERT(sparseSet.count() == 3, "Three elements inserted");

            MANI_TEST_ASSERT(sparseSet.get(10) == 100, "Correct value at 10");
            MANI_TEST_ASSERT(sparseSet.get(42) == 420, "Correct value at 42");
            MANI_TEST_ASSERT(sparseSet.get(7) == 70, "Correct value at 7");
        }

        MANI_TEST(SparseSetDuplicateInsert, "Setting same index twice should not increase count")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(5, 1);
            sparseSet.insert(5, 2);

            MANI_TEST_ASSERT(sparseSet.count() == 1, "Should not duplicate element");
            MANI_TEST_ASSERT(sparseSet.get(5) == 2, "Value should be updated");
        }

        MANI_TEST(SparseSetRemoveInvalid, "Removing non-existing element should fail")
        {
            Mani::SparseSet<int> sparseSet;

            MANI_TEST_ASSERT(sparseSet.removeSwap(99) == Mani::INDEX_NONE, "Removing non-existent index should return false");
        }

        MANI_TEST(SparseSetSwapRemove, "Removing element should maintain valid mapping after swap")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(1, 10);
            sparseSet.insert(2, 20);
            sparseSet.insert(3, 30);

            MANI_TEST_ASSERT(sparseSet.count() == 3, "Three elements");

            sparseSet.removeSwap(2);

            MANI_TEST_ASSERT(sparseSet.count() == 2, "Two elements remain");
            MANI_TEST_ASSERT(sparseSet.getPtr(2) == nullptr, "Removed element should not exist");

            MANI_TEST_ASSERT(sparseSet.get(1) == 10, "Element 1 still valid");
            MANI_TEST_ASSERT(sparseSet.get(3) == 30, "Element 3 still valid");
        }

        MANI_TEST(SparseSetRemoveLast, "Removing last dense element should work")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(10, 100);
            sparseSet.insert(20, 200);

            sparseSet.removeSwap(20);

            MANI_TEST_ASSERT(sparseSet.count() == 1, "One element left");
            MANI_TEST_ASSERT(sparseSet.get(10) == 100, "Remaining element intact");
        }

        MANI_TEST(SparseSetReinsert, "Should allow reinsertion after removal")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(8, 80);
            sparseSet.removeSwap(8);

            MANI_TEST_ASSERT(sparseSet.getPtr(8) == nullptr, "Removed element gone");

            sparseSet.insert(8, 99);

            MANI_TEST_ASSERT(sparseSet.count() == 1, "One element after reinsertion");
            MANI_TEST_ASSERT(sparseSet.get(8) == 99, "Value correctly reinserted");
        }

        MANI_TEST(SparseSetDenseIntegrity, "Dense array should contain exactly active elements")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(3, 30);
            sparseSet.insert(6, 60);
            sparseSet.insert(9, 90);

            Mani::SizeT sum = 0;
            for (int value : sparseSet.getDense())
            {
                sum += value;
            }

            MANI_TEST_ASSERT(sum == 180, "Dense values should match inserted values");
        }

        MANI_TEST(SparseSetRemoveFirstWithSwap, "Removing first element should correctly remap swapped element")
        {
            Mani::SparseSet<int> sparseSet;

            sparseSet.insert(100, 1);
            sparseSet.insert(200, 2);
            sparseSet.insert(300, 3);

            sparseSet.removeSwap(100);

            MANI_TEST_ASSERT(sparseSet.getPtr(100) == nullptr, "100 removed");
            MANI_TEST_ASSERT(sparseSet.get(200) == 2, "200 still valid");
            MANI_TEST_ASSERT(sparseSet.get(300) == 3, "300 still valid");
        }

        MANI_TEST(SparseSetPageBoundary, "Should correctly handle indices across page boundaries")
        {
            Mani::SparseSet<int> sparseSet;

            constexpr Mani::SizeT pageSize = 2048;

            // Insert values around the boundary
            const Mani::SizeT indices[] =
            {
                pageSize - 2,   // 2046
                pageSize - 1,   // 2047 (last of page 0)
                pageSize,       // 2048 (first of page 1)
                pageSize + 1,   // 2049
                pageSize * 2 + 5 // deep into page 2
            };

            // Insert
            for (Mani::SizeT i = 0; i < 5; ++i)
            {
                sparseSet.insert(indices[i], static_cast<int>(indices[i] * 10));
            }

            MANI_TEST_ASSERT(sparseSet.count() == 5, "All boundary elements inserted");

            // Verify all values are accessible
            for (Mani::SizeT i = 0; i < 5; ++i)
            {
                MANI_TEST_ASSERT(
                    sparseSet.get(indices[i]) == static_cast<int>(indices[i] * 10),
                    "Value across page boundary should match"
                );
            }

            // Remove one from first page and one from second page
            sparseSet.removeSwap(pageSize - 1);
            sparseSet.removeSwap(pageSize);

            MANI_TEST_ASSERT(sparseSet.count() == 3, "Two elements removed");

            MANI_TEST_ASSERT(sparseSet.getPtr(pageSize - 1) == nullptr, "Removed page 0 element gone");
            MANI_TEST_ASSERT(sparseSet.getPtr(pageSize) == nullptr, "Removed page 1 element gone");

            // Ensure remaining elements are intact
            MANI_TEST_ASSERT(sparseSet.get(pageSize - 2) == static_cast<int>((pageSize - 2) * 10), "Remaining element valid");
            MANI_TEST_ASSERT(sparseSet.get(pageSize + 1) == static_cast<int>((pageSize + 1) * 10), "Remaining element valid");
            MANI_TEST_ASSERT(sparseSet.get(pageSize * 2 + 5) == static_cast<int>((pageSize * 2 + 5) * 10), "Deep page element valid");

            // Reinsert previously removed values
            sparseSet.insert(pageSize - 1, 1111);
            sparseSet.insert(pageSize, 2222);

            MANI_TEST_ASSERT(sparseSet.count() == 5, "Reinsertion successful");

            MANI_TEST_ASSERT(sparseSet.get(pageSize - 1) == 1111, "Reinserted page 0 element valid");
            MANI_TEST_ASSERT(sparseSet.get(pageSize) == 2222, "Reinserted page 1 element valid");

            // Final dense integrity check
            Mani::SizeT seen = 0;
            for (int value : sparseSet.getDense())
            {
                (void)value;
                ++seen;
            }

            MANI_TEST_ASSERT(seen == sparseSet.count(), "Dense array count matches sparse count");
        }
    }
    MANI_SECTION_END(SparseSet)

    MANI_SECTION_BEGIN(SparseArray, "Sparse Array, a fixed size Sparse Set")
    {
        MANI_TEST(SparseArrayBasics, "Fixed Size SparseArray")
        {
            struct A {};
            constexpr Mani::SizeT capacity = 10;
            constexpr bool allowRealloc = false;
            Mani::SparseArray<A, capacity> arr;
            for (Mani::SizeT i = 0; i < capacity; i++)
            {
                arr.set(i, A());
            }

            // sparset.insert(capacity, A()); // Crashes
        }

        MANI_TEST(FixedSizeSparseArrayShouldHaveStablePointers, "Fixed Size Sparse Sets should have stable pointer")
        {
            struct A
            {
                int value;
            };

            constexpr Mani::SizeT capacity = 10;
            constexpr bool allowRealloc = false;
            Mani::SparseArray<A, capacity> arr;

            // Fill partially
            for (Mani::SizeT i = 0; i < capacity / 2; i++)
            {
                arr.set(i, A{ static_cast<int>(i) });
            }

            // Capture pointers
            A* ptrs[capacity / 2];
            for (Mani::SizeT i = 0; i < capacity / 2; i++)
            {
                ptrs[i] = &arr.get(i);
            }

            // Insert more elements (should not invalidate existing pointers)
            for (Mani::SizeT i = capacity / 2; i < capacity; i++)
            {
                arr.set(i, A{ static_cast<int>(i) });
            }

            // Validate pointers still point to correct data
            for (Mani::SizeT i = 0; i < capacity / 2; i++)
            {
                MANI_TEST_ASSERT(ptrs[i] == &arr.get(i), "Pointer address changed");
                MANI_TEST_ASSERT(ptrs[i]->value == static_cast<int>(i), "Pointer data corrupted");
            }

            // Remove some unrelated elements
            for (Mani::SizeT i = capacity / 2; i < capacity; i++)
            {
                arr.unset(i);
            }

            // Validate again after removals
            for (Mani::SizeT i = 0; i < capacity / 2; i++)
            {
                MANI_TEST_ASSERT(ptrs[i] == &arr.get(i), "Pointer address changed after removal");
                MANI_TEST_ASSERT(ptrs[i]->value == static_cast<int>(i), "Pointer data corrupted after removal");
            }

            arr.unset(0);
            MANI_TEST_ASSERT(arr.toDenseIndex(0) == Mani::INDEX_NONE, "should have discarded index 0");

            for (Mani::SizeT i = 1; i < capacity / 2; i++)
            {
                MANI_TEST_ASSERT(ptrs[i] == &arr.get(i), "Pointer address changed after removal");
                MANI_TEST_ASSERT(ptrs[i]->value == static_cast<int>(i), "Pointer data corrupted after removal");
            }

            arr.set(0, A(5));
            MANI_TEST_ASSERT(arr.toDenseIndex(0) == 0, "should reuse the first index");
            MANI_TEST_ASSERT(arr.get(0).value == 5, "Should be equal to the set value");
        }
    }
    MANI_SECTION_END(SparseArray)
}
MANI_SECTION_END(CopyMoveOperators)