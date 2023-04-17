//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 

#include "MSD.h"

class Application : public MSD::ApplicationCore
{
public:
	Application() {}
	~Application() {}
};

Application* app = nullptr;

int main(int argc, char** argv)
{
	app = new Application();
	app->Run();
	delete app;

	return 0;
}