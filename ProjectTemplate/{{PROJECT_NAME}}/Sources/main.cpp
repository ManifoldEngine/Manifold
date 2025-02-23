#include <Core/CoreFwd.h>

using namespace Mani;

class HelloWorldSystem : public SystemBase
{
    virtual std::string_view getName() const override { return "HelloWorldSystem"; }
    virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
    {
        MANI_LOG(Log, "Hello world!");
    }
};

int main(int argc, char** argv)
{
    Application app;
    app.getSystemContainer().createSystem<HelloWorldSystem>();
    return EXIT_SUCCESS;
}