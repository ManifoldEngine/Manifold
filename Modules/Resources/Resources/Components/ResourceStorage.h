#pragma once

#include <Core/CoreFwd.h>

#include <Resources/IResourceSystemExtension.h>
#include <Resources/IResourceLoader.h>

#include <mutex>

namespace Mani
{
	struct ResourceStorage
	{
		List<IResourceSystemExtension*> extensions;
	};
}