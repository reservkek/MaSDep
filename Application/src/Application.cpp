#include "MSD.h"

using namespace MSD;

class Application : public MSD::ApplicationCore
{
public:
	Application() {}
	~Application() {}
};

int main(int argc, char** argv)
{
	Application* app = new Application();
	app->Run();
	delete app;
}