#include <iostream>
#include "EntityRegistry.h"
#include "RegistryView.h"

using namespace ECSEngine;

struct DataComponent {
	int someData = 5;
};

int main(int argc, char** argv)
{
	EntityRegistry registry;
	EntityId entityId = registry.create();
	if (registry.isValid(entityId))
	{
		std::cout << "entity " << entityId << " created." << std::endl;
	}

	DataComponent* component = registry.addComponent<DataComponent>(entityId);
	if (component != nullptr)
	{
		std::cout << "entity " << entityId << " receiced a component." << std::endl;
	}
	else
	{
		return 1;
	}

	component->someData += 10;

	if (component->someData == registry.getComponent<DataComponent>(entityId)->someData)
	{
		std::cout << "entity " << entityId << "'s DataCompenent received new data. new data: " << component->someData << std::endl;
	}

	if (registry.destroy(entityId))
	{
		std::cout << "entity " << entityId << " destroyed." << std::endl;
	}

	return 0;
}