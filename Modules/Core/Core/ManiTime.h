#pragma once

namespace Mani
{
	struct Time
	{
		float realDelta = 0.f; // in seconds
		float delta = 0.f; // in seconds

		float realElapsed = 0.f; // in seconds
		float elapsed = 0.f; // in seconds

		float scale = 1.f;
	};
}