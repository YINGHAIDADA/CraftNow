#include "CraftNow.h"
class Sandbox : public CraftNow::Application
{
public:
	Sandbox() 
	{

	}
	~Sandbox()
	{

	}

private:

};


CraftNow::Application* CraftNow::CreateApplication() 
{
	return new Sandbox();
}