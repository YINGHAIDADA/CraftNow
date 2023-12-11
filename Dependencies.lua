
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/CraftNow/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/CraftNow/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/CraftNow/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/CraftNow/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/CraftNow/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/CraftNow/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/CraftNow/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/CraftNow/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/CraftNow/vendor/Box2D/include"
IncludeDir["mono"] = "%{wks.location}/CraftNow/vendor/mono/include"
IncludeDir["filewatch"] = "%{wks.location}/CraftNow/vendor/filewatch"
IncludeDir["shaderc"] = "%{VULKAN_SDK}/Include/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{VULKAN_SDK}/Include/spirv_cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["msdfgen"] = "%{wks.location}/CraftNow/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/CraftNow/vendor/msdf-atlas-gen/msdf-atlas-gen"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["mono"] = "%{wks.location}/CraftNow/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"