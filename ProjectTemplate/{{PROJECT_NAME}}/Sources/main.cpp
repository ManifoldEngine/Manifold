#include <Core/CoreFwd.h>

#if !MANI_CONSOLE_APP && MANI_WINDOWS
#include <windows.h>
#endif

using namespace Mani;

class HelloWorldSystem : public SystemBase
{
    virtual std::string_view getName() const override { return "HelloWorldSystem"; }
    virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
    {
        MANI_LOG(Log, "Hello world!");
    }
};

#if MANI_CONSOLE_APP
int main(int argc, char** argv)
#else
int WinMain(
    _In_        HINSTANCE   hInstance, 
    _In_opt_    HINSTANCE   hPrevInstance, 
    _In_        LPSTR       lpCmdLine, 
    _In_        int         nShowCmd
)
#endif
{
    Application app;
    app.getSystemContainer().createSystem<HelloWorldSystem>();
    return EXIT_SUCCESS;
}