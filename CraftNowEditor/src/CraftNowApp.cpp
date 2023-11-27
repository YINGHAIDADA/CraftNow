#include "CraftNow.h"
#include "CraftNow/Core/EntryPoint.h"

#include "EditorLayer.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CraftNow {

	class CraftNowEditor : public Application
	{
	public:
		CraftNowEditor(const ApplicationSpecification& applicationSpecification)
			:Application(applicationSpecification)
		{
			//TODO: 不添加任何层，imgui会出现渲染残影错误
			PushLayer(new EditorLayer());
		}
		~CraftNowEditor()
		{

		}

	private:

	};


	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "CraftNow";
		spec.CommandLineArgs = args;

		return new CraftNowEditor(spec);
	}

}
