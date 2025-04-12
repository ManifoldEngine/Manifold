#pragma once

// describes the current cursor mode
struct Cursor
{
	enum class EMode : uint8_t
	{
		DISABLED = 0, // Captures the cursor and hides it
		HIDDEN, // hides the cursor when above the window
		NORMAL, // 
		CAPTURED, // Captures the cursor but still shows it
	};

	EMode mode = EMode::DISABLED;
};