#include "simpleTests.h"
#include "Events/Event.h"

using namespace Mani;

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}
#endif

ST_SECTION_BEGIN(Core_Events, "Core Events")
{
	ST_TEST(SubscribeAndUnsubscribe, "Should be able to call back the delegate it receives")
	{
		Event<int> someEvent;
		bool wasCalled = false;
		EventHandle handle = someEvent.subscribe([&wasCalled](int i) {
			ST_ASSERT(i == 4, "argument should be equal to what was passed.");
			wasCalled = true;
		});

		someEvent.broadcast(4);
		ST_ASSERT(wasCalled, "delegate should have been called");

		wasCalled = false;
		someEvent.unsubscribe(handle);

		someEvent.broadcast(4);

		ST_ASSERT(!wasCalled, "delegate should have been called.");
	}

	ST_TEST(SubscribeAndUnsubscribeFromFuncRef, "Should be able to call back the delegate it receives, the delegate being a function ref")
	{
		class SomeClass
		{
		public:
			bool wasCalled = false;

			void onSomeEvent(int i)
			{
				ST_ASSERT(i == 4, "argument should be equal to what was passed.");
				wasCalled = true;
			};
		};

		Event<int> someEvent;

		SomeClass someObject;

		EventHandle handle = someEvent.subscribe([&someObject](int i) {
			ST_ASSERT(i == 4, "argument should be equal to what was passed.");
			someObject.onSomeEvent(i);
		});

		someEvent.broadcast(4);
		ST_ASSERT(someObject.wasCalled, "delegate should have been called");

		someObject.wasCalled = false;
		someEvent.unsubscribe(handle);

		someEvent.broadcast(4);

		ST_ASSERT(!someObject.wasCalled, "delegate should have been called.");
	}

	ST_TEST(SubscribeAndUnsubscribeMultipleTimes, "Should be able to call back the all delegates it receives")
	{
		class SomeClass
		{
		public:
			bool wasCalled = false;

			void onSomeEvent(int i)
			{
				ST_ASSERT(i == 4, "argument should be equal to what was passed.");
				wasCalled = true;
			};
		};

		std::vector<SomeClass> listeners;
		for (int i = 0; i < 10; i++)
		{
			listeners.push_back(SomeClass());
		}

		Event<int> someEvent;
		std::vector<EventHandle> handles;
		for (auto& someObject : listeners)
		{
			auto callback = [&someObject](int i) 
							{
								ST_ASSERT(i == 4, "argument should be equal to what was passed.");
								someObject.onSomeEvent(i);
							};
			handles.push_back(someEvent.subscribe(callback));
		}

		someEvent.broadcast(4);

		bool wasCalled = true;
		for (const auto& someObject : listeners)
		{
			wasCalled &= someObject.wasCalled;
		}

		ST_ASSERT(wasCalled, "all delegates should have been called");

		for (const auto& handle : handles)
		{
			someEvent.unsubscribe(handle);
		}
	}

	ST_TEST(SubscribeWithBoundObject, "Should subscribe with a bound object")
	{
		class SomeClass
		{
		public:
			bool wasCalled = false;

			void onSomeEvent(int i)
			{
				ST_ASSERT(i == 4, "argument should be equal to what was passed.");
				wasCalled = true;
			};
		};

		SomeClass someObject;
		Event<int> someEvent;
		// this is an example of how you can bind an object to the function. It's not pretty, but it works.
		someEvent.subscribe(std::bind(&SomeClass::onSomeEvent, &someObject, std::placeholders::_1));
		someEvent.broadcast(4);
		ST_ASSERT(someObject.wasCalled, "someObject should have been called");
	}
}
ST_SECTION_END(Core_Events)