#include <Core/CoreFwd.h>

using namespace Mani;

class HelloSystem : public SystemBase
{
public:
	virtual std::string_view getName() const override { return "HelloSystem"; }
	virtual bool shouldTick(EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, EntityRegistry& registry) override { MANI_LOG(Log, "Hello world!"); }
};

int main(int argc, char** argv)
{
	Application app;
	app.getSystemContainer().createSystem<HelloSystem>();
	app.run();

	return EXIT_SUCCESS;
}