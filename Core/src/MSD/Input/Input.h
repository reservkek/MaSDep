#pragma once

namespace MSD {

	struct MouseCoords {
		double x, y;
	};

	class Input {
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static float GetMouseCoords() { return s_Instance->GetMouseCoordsImpl(); }
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseCoordsImpl() = 0;
	private:
		static Input* s_Instance;
	};

}