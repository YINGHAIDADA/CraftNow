#include "CraftNow.h"

class ExampleLayer : public CraftNow::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{

	}

	void OnUpdate() override
	{
		CN_INFO("ExampleLayer::Update");
	}

	void OnEvent(CraftNow::Event& event) override
	{
		CN_TRACE("{0}", event);
	}
};

class Sandbox : public CraftNow::Application
{
public:
	Sandbox() 
	{
		PushLayer(new ExampleLayer());
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