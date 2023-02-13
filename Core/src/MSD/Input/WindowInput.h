#pragma once

#include "Input.h"

namespace MSD {

	class WindowInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual float GetMouseCoordsImpl() override;
	};

}