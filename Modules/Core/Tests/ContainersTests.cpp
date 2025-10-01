#include <Core/Containers/List.h>
#include <Core/Containers/Map.h>
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
            MANI_TEST_ASSERT(l.atOrDefault(0, -1) == 10, "atOrDefault(0) should return 10");
            MANI_TEST_ASSERT(l.atOrDefault(5, -1) == -1, "Out of bounds atOrDefault should return default (-1)");
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
            Mani::List<int> copySorted = copy.sort(); // const version that returns copy
            MANI_TEST_ASSERT(copySorted == l, "Copy returned from const sort() should match sorted list");
        }

        MANI_TEST(PredicateSort, "Should sort list using custom predicate")
        {
            Mani::List<int> l{ 5, 2, 8, 1, 3 };
            l.sort([](const int& a, const int& b) { return a > b; }); // descending
            MANI_TEST_ASSERT(l[0] == 8, "First element should be 8 after descending sort");
            MANI_TEST_ASSERT(l[4] == 1, "Last element should be 1 after descending sort");
            
            const Mani::List<int> copy = l;
            Mani::List<int> copySorted = copy.sort([](const int& a, const int& b) { return a < b; }); // ascending copy
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
    }
    MANI_SECTION_END(MapOperators)
}
MANI_SECTION_END(CopyMoveOperators)