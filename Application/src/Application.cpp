#include "MSD.h"

class ExampleLayer : public MSD::Layer
{

};

class Application : public MSD::ApplicationCore
{
public:
	Application()
	{
		PushOverlay(new MSD::ImGuiLayer());
	}
	~Application() {}
};

int main(int argc, char** argv)
{
	Application* app = new Application();
	app->Run();
	delete app;
}