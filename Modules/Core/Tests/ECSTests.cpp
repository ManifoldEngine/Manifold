#include <ManiTests/ManiTests.h>
#include <Core/ECS/Registry.h>
#include <Core/ECS/View.h>
#include <Core/ECS/PinnedView.h>
#include <Core/ECS/CommandBuffer.h>

#include <string>
#include <random>
#include <mutex>

namespace ECSTestHelpers
{
	std::string randomString(size_t n) {
		static constexpr char c[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		static thread_local std::mt19937 r{ std::random_device{}() };
		static std::uniform_int_distribution<size_t>d(0, sizeof(c) - 2);
		
		std::string s(n, '\0'); 
		for (auto& x : s)
		{
			x = c[d(r)];
		}
		return s;
	}
}

using namespace Mani;

MANI_SECTION_BEGIN(ECS, "ECS")
{
	MANI_SECTION_BEGIN(Archetype, "Archetype")
	{
		MANI_TEST(EntityLifecycle, "Create, add, mutate, destroy")
		{
			struct Data { int value = 5; };

			ECS::Registry registry;

			ECS::EntityId e = registry.create();
			MANI_TEST_ASSERT(registry.isValid(e), "Entity should be valid");

			Ref<Data> c = registry.add<Data>(e);
			c->value += 10;

			Ref<Data> retrieved = registry.get<Data>(e);
			MANI_TEST_ASSERT(retrieved->value == 15, "Component mutation should persist");

			registry.destroy(e);
			MANI_TEST_ASSERT(!registry.isValid(e), "Entity should be destroyed");
			MANI_TEST_ASSERT(!registry.find<Data>(e).isValid(), "Component should be gone");
		}

		MANI_TEST(AddRemoveComponent, "Add/remove component")
		{
			struct Data {};

			ECS::Registry registry;
			ECS::EntityId e = registry.create();

			registry.add<Data>(e);
			MANI_TEST_ASSERT(registry.has<Data>(e), "Should have component");

			registry.remove<Data>(e);
			MANI_TEST_ASSERT(!registry.has<Data>(e), "Should not have component anymore");
		}

		MANI_TEST(EntityRecycling, "Recycled entities should not leak components")
		{
			struct A {};
			struct B {};

			ECS::Registry registry;

			ECS::EntityId e1 = registry.create();
			registry.add<A>(e1);

			registry.destroy(e1);

			ECS::EntityId e2 = registry.create();

			MANI_TEST_ASSERT(e1 != e2, "Version should differ");
			MANI_TEST_ASSERT(!registry.has<A>(e2), "Recycled entity should not inherit components");

			registry.add<B>(e2);
			MANI_TEST_ASSERT(registry.has<B>(e2), "Should be able to reuse entity cleanly");
		}

		MANI_TEST(ViewIteration, "Basic view iteration")
		{
			struct A {};
			struct B {};

			ECS::Registry registry;

			for (int i = 0; i < 10; ++i)
			{
				ECS::EntityId e = registry.create();
				registry.add<A>(e);
				if (i % 2 == 0)
				{
					registry.add<B>(e);
				}
			}

			int countAB = 0;
			for (auto [entityId, a, b] : ECS::View<A, B>(registry))
			{
				countAB++;
			}

			MANI_TEST_ASSERT(countAB == 5, "Should match entities with A+B");

			int countA = 0;
			for (auto [entityId, a] : ECS::View<A>(registry))
			{
				countA++;
			}

			MANI_TEST_ASSERT(countA == 10, "Should match all A");
		}

		MANI_TEST(CreateDuringIteration, "Creating entities during iteration should not break")
		{
			struct A {};
			struct B {};

			ECS::Registry registry;

			ECS::EntityId e = registry.create();
			registry.add<A>(e);

			for (auto [entityId, a] : ECS::View<A>(registry))
			{
				ECS::EntityId newE = registry.create();
			}

			MANI_TEST_ASSERT(registry.count() == 3, "Should have created additional entities safely");
		}

		MANI_TEST(CommandBufferAdd, "Deferred add should apply after execute")
		{
			struct A {};
			struct B { int value = 0; };

			ECS::Registry registry;

			ECS::EntityId e = registry.create();
			registry.add<A>(e);

			ECS::CommandBuffer cmd(registry);
			for (auto [entityId, a] : ECS::View<A>(registry))
			{
				B& b = cmd.add<B>(entityId);
				b.value = 42;
			}

			MANI_TEST_ASSERT(!registry.has<B>(e), "Should not exist before execute");

			cmd.execute();

			MANI_TEST_ASSERT(registry.has<B>(e), "Should exist after execute");
			MANI_TEST_ASSERT(registry.get<B>(e)->value == 42, "Data should persist");
		}

		MANI_TEST(CommandScopeAdd, "Deferred add should apply after execute when it goes out of scope")
		{
			struct A {};
			struct B { int value = 0; };

			ECS::Registry registry;

			ECS::EntityId e = registry.create();
			registry.add<A>(e);

			{
				ECS::CommandScope cmd(registry);
				ECS::View<A> view(registry);
				for (auto [entityId, a] : view)
				{
					B& b = cmd.add<B>(entityId);
					b.value = 42;
				}
				MANI_TEST_ASSERT(!registry.has<B>(e), "Should not exist before execute");
			}

			MANI_TEST_ASSERT(registry.has<B>(e), "Should exist after execute");
			MANI_TEST_ASSERT(registry.get<B>(e)->value == 42, "Data should persist");
		}

		MANI_TEST(CommandScopeAutoExecute, "CommandScope should auto execute")
		{
			struct A {};
			struct B {};

			ECS::Registry registry;

			ECS::EntityId e = registry.create();
			registry.add<A>(e);

			{
				ECS::CommandScope cmd(registry);
				cmd.add<B>(e);
			}

			MANI_TEST_ASSERT(registry.has<B>(e), "Should have executed on scope end");
		}

		MANI_TEST(ComponentTuples, "addMany / getMany / removeMany")
		{
			struct A {};
			struct B {};
			struct C {};

			ECS::Registry registry;

			ECS::EntityId e = registry.create();

			auto [a, b, c] = registry.addMany<A, B, C>(e);

			MANI_TEST_ASSERT((registry.has<A, B, C>(e)), "should have A, B, and C");

			registry.removeMany<A, C>(e);
			MANI_TEST_ASSERT(!registry.has<A>(e) && !registry.has<C>(e), "Should remove A and C");

			MANI_TEST_ASSERT(!registry.has<A>(e), "A removed");
			MANI_TEST_ASSERT(registry.has<B>(e), "B remains");
		}

		MANI_TEST(ComponentConstruction, "Constructor args forwarding")
		{
			struct Comp
			{
				int a, b, c;
			};

			ECS::Registry registry;
			ECS::EntityId e = registry.create();

			Ref<Comp> comp = registry.add<Comp>(e, 1, 2, 3);

			MANI_TEST_ASSERT(comp->a == 1, "a ok");
			MANI_TEST_ASSERT(comp->b == 2, "b ok");
			MANI_TEST_ASSERT(comp->c == 3, "c ok");
		}

		MANI_TEST(ComponentDestructor, "Destructor should be called on remove")
		{
			static bool called = false;

			struct D
			{
				~D() { called = true; }
			};

			ECS::Registry registry;
			ECS::EntityId e = registry.create();

			registry.add<D>(e);
			registry.remove<D>(e);

			MANI_TEST_ASSERT(called, "Destructor should have run");
		}

		MANI_TEST(ManyEntitiesManyArchetypes, "Should handle many entities spread across multiple archetypes")
		{
			struct A {};
			struct B {};
			struct C {};
			struct D {};

			ECS::Registry registry;

			const int entityCount = 128'374;

			int countA = 0;
			int countAB = 0;
			int countABC = 0;
			int countABCD = 0;

			for (int i = 0; i < entityCount; ++i)
			{
				ECS::EntityId e = registry.create();

				// Deterministic distribution across archetypes
				if (i % 2 == 0)
				{
					registry.add<A>(e);
					countA++;
				}
				if (i % 3 == 0)
				{
					registry.add<B>(e);
				}
				if (i % 5 == 0)
				{
					registry.add<C>(e);
				}
				if (i % 7 == 0)
				{
					registry.add<D>(e);
				}

				// Track exact combinations
				if (registry.has<A, B>(e)) countAB++;
				if (registry.has<A, B, C>(e)) countABC++;
				if (registry.has<A, B, C, D>(e)) countABCD++;
			}

			// Validate via views
			int viewA = 0;
			for (auto [entityId, a] : ECS::View<A>(registry))
			{
				viewA++;
			}
			MANI_TEST_ASSERT(viewA == countA, "View<A> count mismatch");

			int viewAB = 0;
			for (auto [entityId, a, b] : ECS::View<A, B>(registry))
			{
				viewAB++;
			}
			MANI_TEST_ASSERT(viewAB == countAB, "View<A,B> count mismatch");

			int viewABC = 0;
			for (auto [entityId, a, b, c] : ECS::View<A, B, C>(registry))
			{
				viewABC++;
			}
			MANI_TEST_ASSERT(viewABC == countABC, "View<A,B,C> count mismatch");

			int viewABCD = 0;
			for (auto [entityId, a, b, c, d] : ECS::View<A, B, C, D>(registry))
			{
				viewABCD++;
			}
			MANI_TEST_ASSERT(viewABCD == countABCD, "View<A,B,C,D> count mismatch");

			MANI_TEST_ASSERT(registry.count() == entityCount + 1, "Total entity count mismatch (+1 for singleton)");
		}

		MANI_TEST(ArchetypeMemoryMovement, "Dynamic memory should be preserved as entities are moved from one archetype to the other")
		{
			struct D
			{
				struct SomeData
				{
					int a = 420;
					int b = 69;
					int c = 42069;
					std::string d = "coucou";
				};

				//std::mutex mutex; doesn't compile
				Mani::List<SomeData> data;
			};

			struct E
			{
				int a = 1;
				int b = 2;
				int c = 3;
			};

			ECS::Registry registry;
			ECS::EntityId id = registry.create();

			{
				Ref<D> d = registry.add<D>(id);
				d->data.add(D::SomeData());
				d->data.add(D::SomeData());
				d->data.add(D::SomeData());
			}

			registry.add<E>(id);

			{
				Ref<D> d = registry.get<D>(id);
				for (const auto& data : d->data)
				{
					MANI_TEST_ASSERT(data.a == 420, "should be equal");
					MANI_TEST_ASSERT(data.b == 69, "should be equal");
					MANI_TEST_ASSERT(data.c == 42069, "should be equal");
					MANI_TEST_ASSERT(data.d == "coucou", "should be equal");
				}
			}

			{
				Ref<E> e = registry.get<E>(id);
				MANI_TEST_ASSERT(e->a == 1, "should be equal");
				MANI_TEST_ASSERT(e->b == 2, "should be equal");
				MANI_TEST_ASSERT(e->c == 3, "should be equal");
			}

			// should also work with singletons

			Mani::Array<std::string, 3> strings
			{
				ECSTestHelpers::randomString(512),
				ECSTestHelpers::randomString(512),
				ECSTestHelpers::randomString(512),
			};

			{
				Ref<D> d = registry.addSingle<D>();
				d->data.add(D::SomeData{ .d = strings[0] });
				d->data.add(D::SomeData{ .d = strings[1] });
				d->data.add(D::SomeData{ .d = strings[2] });
			}

			registry.addSingle<E>();
			registry.addSingle<float>();

			{
				Ref<D> d = registry.getSingle<D>();
				for (SizeT i = 0; i < d->data.count(); i++)
				{
					MANI_TEST_ASSERT(d->data[i].a == 420, "should be equal");
					MANI_TEST_ASSERT(d->data[i].b == 69, "should be equal");
					MANI_TEST_ASSERT(d->data[i].c == 42069, "should be equal");
					MANI_TEST_ASSERT(d->data[i].d == strings[i], "should be equal");
				}
			}

			{
				Ref<E> e = registry.getSingle<E>();
				MANI_TEST_ASSERT(e->a == 1, "should be equal");
				MANI_TEST_ASSERT(e->b == 2, "should be equal");
				MANI_TEST_ASSERT(e->c == 3, "should be equal");
			}
		}

		MANI_TEST(GetEntityIdFromView, "Should safely get an entityId from a view's iterator")
		{
			struct A {};

			{
				ECS::Registry registry;
				const ECS::EntityId entityId = registry.create();
				registry.add<A>(entityId);

				{
					ECS::ConstView<A> view(registry);
					MANI_TEST_ASSERT(view.begin().getEntityId() == entityId, "EntityId should be included in the view");
				}

				{
					ECS::ConstView<> view(registry);
					MANI_TEST_ASSERT(view.begin().getEntityId() == entityId, "EntityId should be included in the empty view");
				}
			}

			{
				ECS::Registry registry;

				{
					ECS::View<> view(registry);
					MANI_TEST_ASSERT(view.begin().getEntityId() == ECS::INVALID_ID, "No entity should exist in the view");
				}

				struct B {};
				const ECS::EntityId entityId = registry.create();
				registry.add<B>(entityId);
				{
					ECS::View<A> view(registry);
					MANI_TEST_ASSERT(view.begin().getEntityId() == ECS::INVALID_ID, "entity should not be included in the view");
				}
			}
		}

		MANI_TEST(UsingDynamicallyAllocatedDataStructures, "Should safely handle dynamically allocated data structures")
		{
			struct A
			{
				Mani::List<double> data;
			};

			struct B {};

			ECS::Registry registry;
			const auto e1 = registry.create();
			{
				Ref<A> a1 = registry.add<A>(e1);

				a1->data.add(1);
				a1->data.add(2);
				a1->data.add(3);
			}

			const auto e2 = registry.create();
			registry.add<A>(e2);

			{
				SizeT i = 0;
				const Mani::Array<double, 4> ref{ 1, 2, 3, 4 };
				Ref<A> a1 = registry.get<A>(e1);
				for (auto& value : a1->data)
				{
					MANI_TEST_ASSERT(a1->data[i] == ref[i], "should be equal");
					i++;
				}
			}

			registry.add<B>(e1);
			registry.add<B>(e2);

			{
				SizeT i = 0;
				const Mani::Array<double, 4> ref{ 1, 2, 3, 4 };
				Ref<A> a1 = registry.get<A>(e1);
				for (auto& value : a1->data)
				{
					MANI_TEST_ASSERT(a1->data[i] == ref[i], "should be equal");
					i++;
				}
			}
		}

		MANI_TEST(DefaultComponentRefShouldBeInvalid, "Default component ref should be invalid")
		{
			struct A {};
			MANI_TEST_ASSERT(!Ref<A>().isValid(), "default ref should be invalid");
		}

		MANI_TEST(ShoulNotAllowUsingDanglingReference, "Should not allow to use a dangling reference after adding a component to an entity")
		{
			struct A {};
			struct B {};
			struct C {};
			struct D {};

			ECS::Registry registry;

			{
				const ECS::EntityId e = registry.create();
				Ref<A> a = registry.add<A>(e);
				MANI_TEST_ASSERT(a.isValid(), "a should be valid");
				Ref<B> b = registry.add<B>(e);
				MANI_TEST_ASSERT(!a.isValid(), "a should be invalid after adding B");
				registry.remove<B>(e);
				MANI_TEST_ASSERT(!b.isValid(), "b should be invalid after removing B");
				registry.destroy(e);
			}

			{
				const ECS::EntityId e = registry.create();
				registry.add<A>(e);
				Ref<A> a = registry.get<A>(e);
				registry.addMany<B, C, D>(e);
				MANI_TEST_ASSERT(!a.isValid(), "a should be invalid after adding the components");
				Ref<C> c = registry.get<C>(e);
				registry.removeMany<C, A, D>(e);
				MANI_TEST_ASSERT(!c.isValid(), "a should be invalid after adding the components");
				MANI_TEST_ASSERT(registry.get<B>(e).isValid(), "b should still be valid");
				registry.destroy(e);
			}

			{
				const ECS::EntityId e = registry.create();
				Ref<A> a = registry.add<A>(e);
				registry.destroy(e);
				MANI_TEST_ASSERT(!a.isValid(), "a should be invalid after deleting e");
			}

			{
				const ECS::EntityId e1 = registry.create();
				Ref<A> a1 = registry.add<A>(e1);
				const ECS::EntityId e2 = registry.create();
				registry.add<A>(e2);
				registry.destroy(e2);

				MANI_TEST_ASSERT(!a1.isValid(), "a should be invalid after deleting e2 (archetype remove swap)");
			}

			{
				const ECS::EntityId e = registry.create();
				registry.add<A>(e);

				Ref<A> a1 = registry.get<A>(e);
				registry.add<B>(e);

				Ref<A> a2 = registry.get<A>(e);

				MANI_TEST_ASSERT(!a1.isValid(), "old ref should be invalid");
				MANI_TEST_ASSERT(a2.isValid(), "new ref should be valid");
			}

			{
				const ECS::EntityId e = registry.create();
				Ref<A> a1 = registry.add<A>(e);
				Ref<A> a2 = registry.get<A>(e);

				registry.add<B>(e);

				MANI_TEST_ASSERT(!a1.isValid(), "a1 invalid");
				MANI_TEST_ASSERT(!a2.isValid(), "a2 invalid");
			}

			{
				const ECS::EntityId e1 = registry.create();
				const ECS::EntityId e2 = registry.create();

				Ref<A> a1 = registry.add<A>(e1);
				registry.add<A>(e2); // unrelated entity

				MANI_TEST_ASSERT(a1.isValid(), "should be invalid due to global version change");
			}

			{
				ECS::Registry registry;

				const ECS::EntityId e = registry.create();
				Ref<A> a = registry.add<A>(e);

				// Force pool growth
				for (int i = 0; i < 10'000; ++i)
				{
					auto ei = registry.create();
					registry.add<A>(ei);
				}

				MANI_TEST_ASSERT(!a.isValid(), "should be invalid after pool reallocation");
			}

			{
				ECS::Registry registry;

				const ECS::EntityId e1 = registry.create();
				Ref<A> a = registry.add<A>(e1);

				const ECS::EntityId e2 = registry.create();
				// Force many archetypes
				registry.add<B>(e2);
				registry.add<C>(e2);
				registry.add<D>(e2);

				MANI_TEST_ASSERT(!a.isValid(), "should be invalid after archetype container realloc");
			}

			{
				const ECS::EntityId e = registry.create();
				registry.addMany<A, B, C>(e);

				Ref<A> a = registry.get<A>(e);
				Ref<B> b = registry.get<B>(e);

				registry.remove<C>(e);

				MANI_TEST_ASSERT(!a.isValid(), "a invalid due to archetype move");
				MANI_TEST_ASSERT(!b.isValid(), "b invalid due to archetype move");
			}

			{
				const ECS::EntityId e1 = registry.create();
				const ECS::EntityId e2 = registry.create();

				registry.add<A>(e1);
				Ref<A> a2 = registry.add<A>(e2);

				registry.destroy(e2); // no swap actually happens

				MANI_TEST_ASSERT(!a2.isValid(), "should still invalidate even without swap");
			}

			{
				const ECS::EntityId e = registry.create();
				Ref<A> a = registry.add<A>(e);

				{
					registry.add<B>(e);
				}

				// Use later
				MANI_TEST_ASSERT(!a.isValid(), "ref must not survive structural scope");
			}

			{
				const ECS::EntityId e = registry.create();
				registry.add<A>(e);

				if (Ref<A> a = registry.find<A>(e))
				{
					MANI_TEST_ASSERT(a.isValid(), "should be able to use find as a bool operator");
				}
				else
				{
					MANI_TEST_ASSERT(false, "should be able to use find as a bool operator");
				}

				if (Ref<B> b = registry.find<B>(e))
				{
					MANI_TEST_ASSERT(false, "should be able to use find as a bool operator");
				}
				else
				{
					MANI_TEST_ASSERT(true, "should be able to use find as a bool operator");
				}
			}

			{
				const ECS::EntityId e = registry.create();
				auto [a, b, c] = registry.addMany<A, B, C>(e);

				registry.remove<C>(e);

				MANI_TEST_ASSERT(!a.isValid(), "a invalid due to archetype move");
				MANI_TEST_ASSERT(!b.isValid(), "b invalid due to archetype move");
			}
		}
	}
	MANI_SECTION_END(Archetype)

	MANI_SECTION_BEGIN(Pinned, "Pinned")
	{
		MANI_TEST(PinnedComponentBasics, "Should be able to add pinned component and iterated on them")
		{
			struct A {};
			struct B { int value = 0; };
			struct C { Mani::List<std::string> names; };

			ECS::Registry registry;

			const ECS::EntityId e = registry.create();

			registry.addPinned<A>(e);
			MANI_TEST_ASSERT(registry.hasPinned<A>(e), "Should have A pinned");
			MANI_TEST_ASSERT(!registry.has<A>(e), "Should have A as a coponent");
			MANI_TEST_ASSERT(registry.findPinned<A>(e) != nullptr, "Should find A");

			{
				B& b = registry.addPinned<B>(e, 69);
				MANI_TEST_ASSERT(b.value == 69, "should construct B with the correct value");
			}

			for (const auto [entityId, a, b] : ECS::PinnedView<A, B>(registry))
			{
				MANI_TEST_ASSERT(e == entityId, "should view the entity and its pinned component");
				MANI_TEST_ASSERT(b.value == 69, "should view B with the correct value");
			}

			registry.removePinned<A>(e);
			MANI_TEST_ASSERT(!registry.hasPinned<A>(e), "Should not have A");
			registry.add<A>(e);
			MANI_TEST_ASSERT(registry.has<A>(e), "Should have A in its archetype");

			for (const auto [entityId, a, b] : ECS::PinnedView<A, B>(registry))
			{
				MANI_TEST_ASSERT(false, "should not find anything since we unpinned A");
			}

			for (const auto [entityId, b] : ECS::PinnedView<B>(registry))
			{
				MANI_TEST_ASSERT(e == entityId, "should view the entity and its pinned component");
				MANI_TEST_ASSERT(b.value == 69, "should view B with the correct value");
				MANI_TEST_ASSERT(registry.has<A>(e), "should have A (this time as an archetype");
			}
		}

		MANI_TEST(ShouldNotAllowToPinAnExistingComponent, "Should not allow to pin an existing component")
		{
			struct A {};
			struct B {};

			ECS::Registry registry;

			ECS::EntityId e = registry.create();
			registry.add<A>(e);
			// registry.addPinned<A>(e); asserts

			registry.addPinned<B>(e);
			// registry.add<B>(e); asserts
		}
	}
	MANI_SECTION_END(Pinned)

	MANI_SECTION_BEGIN(LazyRefTests, "Lazy Ref Tests")
	{
		MANI_TEST(LazyRefBasics, "Lazy ref basics")
		{
			struct A { int value = 0; };
			struct B { int value = 0; };

			ECS::Registry registry;
			const ECS::EntityId e = registry.create();
			registry.add<A>(e);

			LazyRef<A> aRef(e, registry);
			aRef->value = 420;

			registry.add<B>(e, 69);

			MANI_ASSERT(aRef->value == 420, "should be able to access stable ref even after changing archetype");
		}
	}
	MANI_SECTION_END(LazyRefTests)
}
MANI_SECTION_END(ECS)