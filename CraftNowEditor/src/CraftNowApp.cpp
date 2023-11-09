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
		CraftNowEditor()
		{
			//TODO:不添加任何层，imgui会出现渲染残影错误
			PushLayer(new EditorLayer());
		}
		~CraftNowEditor()
		{

		}

	private:

	};


	Application* CreateApplication() 
	{
		return new CraftNowEditor();
	}

}