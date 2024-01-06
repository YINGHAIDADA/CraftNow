#include "cnpch.h"
#include "SceneHierarchyPanel.h"
#include "CraftNow/Scene/Components.h"

#include "CraftNow/Script/ScriptEngine.h"
#include "../UI/UI.h"

#include "CraftNow/Asset/AssetManager.h"
#include "CraftNow/Asset/AssetMetadata.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>


// 移除std::strncpy()警告
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace CraftNow {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin(u8"场景层级");

		if (m_Context)
		{
			//消除4996警告
			#pragma warning(suppress : 4996)
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID , m_Context.get() };
					DrawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			// 右键空白区域
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::BeginMenu(u8"创建 ..."))
				{
					if (ImGui::MenuItem(u8"创建 空"))
					{
						m_Context->CreateEntity(u8"空");
					}
					if (ImGui::MenuItem(u8"创建 相机"))
					{
						auto& cameraT = m_Context->CreateEntity(u8"相机");
						cameraT.AddComponent<CameraComponent>();
					}
					if (ImGui::MenuItem(u8"创建 精灵"))
					{
						auto& spriteT = m_Context->CreateEntity(u8"精灵");
						spriteT.AddComponent<SpriteRendererComponent>();
					}
					if (ImGui::MenuItem(u8"创建 圆"))
					{
						auto& spriteT = m_Context->CreateEntity(u8"圆");
						spriteT.AddComponent<CircleRendererComponent>();
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

		}
		ImGui::End();

		ImGui::Begin(u8"属性");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem(u8"删除 实体"))
				entityDeleted = true;
			if (ImGui::MenuItem(u8"复制 实体"))
				m_Context->DuplicateEntity(entity);
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 50.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("-", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup(u8"组件设置");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup(u8"组件设置"))
			{
				if (ImGui::MenuItem(u8"移除组件"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button(u8"添加组件"))
			ImGui::OpenPopup(u8"添加组件");

		if (ImGui::BeginPopup(u8"添加组件"))
		{
			DisplayAddComponentEntry<CameraComponent>(u8"相机");
			DisplayAddComponentEntry<ScriptComponent>(u8"脚本");
			DisplayAddComponentEntry<SpriteRendererComponent>(u8"精灵");
			DisplayAddComponentEntry<CircleRendererComponent>(u8"圆");
			DisplayAddComponentEntry<Rigidbody2DComponent>(u8"2D刚体");
			DisplayAddComponentEntry<BoxCollider2DComponent>(u8"2D方块 碰撞体");
			DisplayAddComponentEntry<CircleCollider2DComponent>(u8"2D圆 碰撞体");
			//DisplayAddComponentEntry<TextComponent>("Text Component");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>(u8"变换", entity, [](auto& component)
			{
				DrawVec3Control(u8"位置", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control(u8"旋转", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control(u8"缩放", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>(u8"相机", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox(u8"主相机", &component.Primary);
				if (component.Primary)
				{
					//TODO: 清除其他相机的Primary设置
				}

				const char* projectionTypeStrings[] = { u8"透视", u8"正交" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo(u8"相机类型", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat(u8"垂直FOV", &perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat(u8"最近距离", &perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat(u8"最远距离", &perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat(u8"尺寸", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat(u8"最近距离", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat(u8"最远距离", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox(u8"固定纵横比", &component.FixedAspectRatio);
				}
			});

		DrawComponent<SpriteRendererComponent>(u8"精灵", entity, [](auto& component)
			{
				ImGui::ColorEdit4(u8"颜色", glm::value_ptr(component.Color));

				
				std::string label = "None";
				bool isTextureValid = false;
				if (component.Texture != 0)
				{
					if (AssetManager::IsAssetHandleValid(component.Texture)
						&& AssetManager::GetAssetType(component.Texture) == AssetType::Texture2D)
					{
						const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Texture);
						label = metadata.FilePath.filename().string();
						isTextureValid = true;
					}
					else
					{
						label = "Invalid";
					}
				}

				ImVec2 buttonLabelSize = ImGui::CalcTextSize(label.c_str());
				buttonLabelSize.x += 20.0f;
				float buttonLabelWidth = glm::max<float>(100.0f, buttonLabelSize.x);

				ImGui::Button(label.c_str(), ImVec2(buttonLabelWidth, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *(AssetHandle*)payload->Data;
						if (AssetManager::GetAssetType(handle) == AssetType::Texture2D)
						{
							component.Texture = handle;
						}
						else
						{
							CN_CORE_WARN("Wrong asset type!");
						}

					}
					ImGui::EndDragDropTarget();
				}

				if (isTextureValid)
				{
					ImGui::SameLine();
					ImVec2 xLabelSize = ImGui::CalcTextSize("X");
					float buttonSize = xLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;
					if (ImGui::Button("X", ImVec2(buttonSize, buttonSize)))
					{
						component.Texture = 0;
					}
				}

				ImGui::SameLine();
				ImGui::Text(u8"纹理");

				ImGui::DragFloat(u8"平铺系数", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});

		DrawComponent<ScriptComponent>(u8"脚本", entity, [entity, scene = m_Context](auto& component) mutable
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

				UI::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

				if (ImGui::InputText(u8"类名", buffer, sizeof(buffer)))
				{
					component.ClassName = buffer;
					return;
				}

				// Fields
				bool sceneRunning = scene->IsRunning();
				if (sceneRunning)
				{
					Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);

								ImGui::Text("%s: %f", name.c_str(), data);
								/*if (ImGui::DragFloat(name.c_str(), &data))
								{
									scriptInstance->SetFieldValue(name, data);
								}*/
							}

							if (field.Type == ScriptFieldType::Vector3)
							{
								glm::vec3 data = scriptInstance->GetFieldValue<glm::vec3>(name);

								ImGui::Text("%s: (%.2f, %.2f, %.2f)", name.c_str(), data.x, data.y, data.z);
								/*if (ImGui::DragFloat(name.c_str(), &data))
								{
									scriptInstance->SetFieldValue(name, data);
								}*/
							}
						}
					}
				}
				else
				{
					if (scriptClassExists)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
						const auto& fields = entityClass->GetFields();

						auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
						for (const auto& [name, field] : fields)
						{
							// Field has been set in editor
							if (entityFields.find(name) != entityFields.end()) //经过初始化的从此处显示
							{
								ScriptFieldInstance& scriptField = entityFields.at(name);

								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = scriptField.GetValue<float>();
									//if (ImGui::DragFloat(name.c_str(), &data))
									if(ImGui::SliderFloat(name.c_str(), &data, 0.0f, 0.03f))
										scriptField.SetValue(data);
								}

								if (field.Type == ScriptFieldType::Vector3)
								{
									glm::vec3 data = scriptField.GetValue<glm::vec3>();

									ImGui::Text("%s: (%.2f, %.2f, %.2f)", name.c_str(), data.x, data.y, data.z);
									
								}
							}
							else
							{
								// Display control to set it maybe
								if (field.Type == ScriptFieldType::Float)
								{
									float data = 0.0f;
									if (ImGui::SliderFloat(name.c_str(), &data, 0.0f, 0.03f))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}

								if (field.Type == ScriptFieldType::Vector3)
								{
									glm::vec3 data = { 0.0f,0.0f,0.0f };

									ImGui::Text("%s: (%.2f, %.2f, %.2f)", name.c_str(), data.x, data.y, data.z);
									/*if (ImGui::DragFloat(name.c_str(), &data))
									{
										scriptInstance->SetFieldValue(name, data);
									}*/
								}
							}
						}
					}
				}
			});

		DrawComponent<CircleRendererComponent>(u8"圆", entity, [](auto& component)
			{
				ImGui::ColorEdit4(u8"颜色", glm::value_ptr(component.Color));
				ImGui::DragFloat(u8"厚度", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"渐变", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		DrawComponent<Rigidbody2DComponent>(u8"2D刚体", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { u8"静态", u8"动态", u8"运动学" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo(u8"类型", currentBodyTypeString))
				{
					for (int i = 0; i <= 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox(u8"固定旋转", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>(u8"2D 方形碰撞体", entity, [](auto& component)
			{
				ImGui::DragFloat2(u8"偏移", glm::value_ptr(component.Offset));
				ImGui::DragFloat2(u8"尺寸", glm::value_ptr(component.Size));
				ImGui::DragFloat(u8"密度", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"摩擦系数", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"弹力系数", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"弹力门限", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>(u8"2D 圆形碰撞体", entity, [](auto& component)
			{
				ImGui::DragFloat2(u8"偏移", glm::value_ptr(component.Offset));
				ImGui::DragFloat(u8"半径", &component.Radius);
				ImGui::DragFloat(u8"密度", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"摩擦系数", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"弹力系数", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat(u8"弹力门限", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<TextComponent>("Text Renderer", entity, [](auto& component)
			{
				ImGui::InputTextMultiline("Text String", &component.TextString);
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
				ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
			});
	}

}
