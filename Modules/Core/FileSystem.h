#include <Core/Core.h>
#include <string>

namespace ECSEngine
{
	class Core_API FileSystem
	{
	public:
		static bool tryReadFile(const std::string& filePath, std::string& outResult);
	};
}