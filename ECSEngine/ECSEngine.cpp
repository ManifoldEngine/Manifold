#include "ECSEngine.h"

#include <iostream>
#include <ECS/EntityRegistry.h>

using namespace ECSEngine;

struct DataComponent {
	int SomeData = 5;
};

int main()
{
	Entity entity;
	EntityRegistry registry;
	uint32_t entityId = registry.Create();
	DataComponent component;
	registry.AddComponent<DataComponent>(entityId, component);
	registry.Destroy(entityId);

	return 0;
}