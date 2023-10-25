#include "CraftNow.h"

#include "imgui/imgui.h"

class ExampleLayer : public CraftNow::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{

	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnUpdate() override
	{
		//CN_INFO("ExampleLayer::Update");
	}

	void OnEvent(CraftNow::Event& event) override
	{
		CN_TRACE("{0}:{1}", m_DebugName, event);
	}
};

class Sandbox : public CraftNow::Application
{
public:
	Sandbox() 
	{
		//PushLayer(new ExampleLayer());
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