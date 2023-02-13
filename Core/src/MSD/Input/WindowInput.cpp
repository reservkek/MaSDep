#include "msdpch.h"

#include "WindowInput.h"
#include <GLFW/glfw3.h>

namespace MSD {

	Input* Input::s_Instance = new WindowInput();

	bool WindowInput::IsKeyPressedImpl(int keycode) {
		auto state = glfwGetKey(glfwGetCurrentContext(), keycode);
		return state == GLFW_PRESS or state == GLFW_REPEAT;
	}

	bool WindowInput::IsMouseButtonPressedImpl(int button) {
		auto state = glfwGetMouseButton(glfwGetCurrentContext(), button);
		return state == GLFW_PRESS;
	}

	float WindowInput::GetMouseCoordsImpl() {
		double mouseX, mouseY;
		glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
		return (float)mouseX;
	}

}
