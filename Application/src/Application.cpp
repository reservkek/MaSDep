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

Application* app = nullptr;


int main(int argc, char** argv)
{
	app = new Application();
	app->Run();
	delete app;
}