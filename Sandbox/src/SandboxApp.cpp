﻿#include "CraftNow.h"
#include "CraftNow/Core/EntryPoint.h"

#include "Sandbox2D.h"
#include "ExampleLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sandbox : public CraftNow::Application
{
public:
	Sandbox() 
	{
		//TODO:不添加任何层，imgui会出现渲染残影错误
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
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