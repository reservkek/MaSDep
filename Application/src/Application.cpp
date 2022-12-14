#include "MSD.h"

using namespace MSD;

class ExampleLayer : public Layer
{

};

class Application : public MSD::ApplicationCore
{
public:
	Application()
	{
		PushOverlay(new ImGuiLayer());
	}
	~Application() {}
};

int main(int argc, char** argv)
{
	Application* app = new Application();
	app->Run();
	delete app;
}