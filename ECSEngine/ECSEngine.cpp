#include "ECSEngine.h"

#include <iostream>
#include <ECS/EntityRegistry.h>
#include <ECS/RegistryView.h>

using namespace ECSEngine;

struct DataComponent {
	int SomeData = 5;
};

int main()
{
	EntityRegistry registry;
	EntityId entityId = registry.Create();
	if (registry.IsValid(entityId))
	{
		std::cout << "entity " << entityId << " created." << std::endl;
	}

	DataComponent* component = registry.AddComponent<DataComponent>(entityId);
	if (component != nullptr)
	{
		std::cout << "entity " << entityId << " receiced a component." << std::endl;
	}
	else
	{
		return 1;
	}

	component->SomeData += 10;
	
	if (component->SomeData == registry.GetComponent<DataComponent>(entityId)->SomeData)
	{
		std::cout << "entity " << entityId << "'s DataCompenent received new data. new data: " << component->SomeData << std::endl;
	}

	if (registry.Destroy(entityId))
	{
		std::cout << "entity " << entityId << " destroyed." << std::endl;
	}

	return 0;
}