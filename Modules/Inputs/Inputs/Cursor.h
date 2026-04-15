#pragma once

namespace Mani
{
	// describes the current cursor mode
	struct Cursor
	{
		enum class EMode : uint8_t
		{
			Disabled = 0, // Captures the cursor and hides it
			Hidden, // hides the cursor when above the window
			Normal, // 
			Captured, // Captures the cursor but still shows it
		};

		EMode mode = EMode::Disabled;
	};
}