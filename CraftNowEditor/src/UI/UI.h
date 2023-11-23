#pragma once
#include "CraftNow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <memory>

namespace CraftNow::UI {
	namespace Colors
	{
		static inline float Convert_sRGB_FromLinear(float theLinearValue);
		static inline float Convert_sRGB_ToLinear(float thesRGBValue);
		ImVec4 ConvertFromSRGB(ImVec4 colour);
		ImVec4 ConvertToSRGB(ImVec4 colour);

		// To experiment with editor theme live you can change these constexpr into static
		// members of a static "Theme" class and add a quick ImGui window to adjust the colour values
		namespace Theme
		{
			constexpr auto accent = IM_COL32(236, 158, 36, 255);
			constexpr auto highlight = IM_COL32(39, 185, 242, 255);
			constexpr auto niceBlue = IM_COL32(83, 232, 254, 255);
			constexpr auto compliment = IM_COL32(78, 151, 166, 255);
			constexpr auto background = IM_COL32(56, 56, 56, 255);
			constexpr auto backgroundDark = IM_COL32(36, 36, 36, 255);
			constexpr auto titlebar = IM_COL32(21, 21, 21, 255);
			constexpr auto propertyField = IM_COL32(15, 15, 15, 255);
			constexpr auto text = IM_COL32(192, 192, 192, 255);
			constexpr auto textBrighter = IM_COL32(210, 210, 210, 255);
			constexpr auto textDarker = IM_COL32(128, 128, 128, 255);
			constexpr auto textError = IM_COL32(230, 51, 51, 255);
			constexpr auto muted = IM_COL32(77, 77, 77, 255);
			constexpr auto groupHeader = IM_COL32(47, 47, 47, 255);
			constexpr auto selection = IM_COL32(237, 192, 119, 255);
			constexpr auto selectionMuted = IM_COL32(237, 201, 142, 23);
			constexpr auto backgroundPopup = IM_COL32(50, 50, 50, 255);
			constexpr auto validPrefab = IM_COL32(82, 179, 222, 255);
			constexpr auto invalidPrefab = IM_COL32(222, 43, 43, 255);
			constexpr auto missingMesh = IM_COL32(230, 102, 76, 255);
			constexpr auto meshNotSet = IM_COL32(250, 101, 23, 255);
		}
	}

	namespace Colors
	{
		inline float Convert_sRGB_FromLinear(float theLinearValue)
		{
			return theLinearValue <= 0.0031308f
				? theLinearValue * 12.92f
				: glm::pow<float>(theLinearValue, 1.0f / 2.2f) * 1.055f - 0.055f;
		}

		inline float Convert_sRGB_ToLinear(float thesRGBValue)
		{
			return thesRGBValue <= 0.04045f
				? thesRGBValue / 12.92f
				: glm::pow<float>((thesRGBValue + 0.055f) / 1.055f, 2.2f);
		}

		inline ImVec4 ConvertFromSRGB(ImVec4 colour)
		{
			return ImVec4(Convert_sRGB_FromLinear(colour.x),
				Convert_sRGB_FromLinear(colour.y),
				Convert_sRGB_FromLinear(colour.z),
				colour.w);
		}

		inline ImVec4 ConvertToSRGB(ImVec4 colour)
		{
			return ImVec4(std::pow(colour.x, 2.2f),
				glm::pow<float>(colour.y, 2.2f),
				glm::pow<float>(colour.z, 2.2f),
				colour.w);
		}

		inline ImU32 ColorWithValue(const ImColor& color, float value)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(value, 1.0f));
		}

		inline ImU32 ColorWithSaturation(const ImColor& color, float saturation)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(saturation, 1.0f), val);
		}

		inline ImU32 ColorWithHue(const ImColor& color, float hue)
		{
			const ImVec4& colRow = color.Value;
			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, h, s, v);
			return ImColor::HSV(std::min(hue, 1.0f), s, v);
		}

		inline ImU32 ColorWithMultipliedValue(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
		}

		inline ImU32 ColorWithMultipliedSaturation(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(sat * multiplier, 1.0f), val);
		}

		inline ImU32 ColorWithMultipliedHue(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(std::min(hue * multiplier, 1.0f), sat, val);
		}
	}

	void ShiftCursorX(float distance);
	void ShiftCursorY(float distance);
	void ShiftCursor(float x, float y);

	ImRect GetItemRect();
	ImRect RectExpanded(const ImRect& rect, float x, float y);
	ImRect RectOffset(const ImRect& rect, float x, float y);
	ImRect RectOffset(const ImRect& rect, ImVec2 xy);

	void DrawButtonImage(const Ref<Texture>& imageNormal, const Ref<Texture>& imageHovered, const Ref<Texture>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax);

	void DrawButtonImage(const Ref<Texture>& imageNormal, const Ref<Texture>& imageHovered, const Ref<Texture>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle);

	void DrawButtonImage(const Ref<Texture>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax);

	void DrawButtonImage(const Ref<Texture>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle);

	void DrawButtonImage(const Ref<Texture>& imageNormal, const Ref<Texture>& imageHovered, const Ref<Texture>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	void DrawButtonImage(const Ref<Texture>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	void RenderWindowOuterBorders(ImGuiWindow* window);

	bool UpdateWindowManualResize(ImGuiWindow* window, ImVec2& newSize, ImVec2& newPosition);

	// Menubar with custom rectangle
	bool BeginMenubar(const ImRect& barRectangle);
	void EndMenubar();

	bool ButtonCentered(const char* label, const ImVec2& size = ImVec2(0, 0));

	// Utilities
	class ScopedStyle
	{
	public:
		ScopedStyle(const ScopedStyle&) = delete;
		ScopedStyle operator=(const ScopedStyle&) = delete;
		template<typename T>
		ScopedStyle(ImGuiStyleVar styleVar, T value) { ImGui::PushStyleVar(styleVar, value); }
		~ScopedStyle() { ImGui::PopStyleVar(); }
	};

	class ScopedColor
	{
	public:
		ScopedColor(const ScopedColor&) = delete;
		ScopedColor operator=(const ScopedColor&) = delete;
		template<typename T>
		ScopedColor(ImGuiCol ColorId, T Color) { ImGui::PushStyleColor(ColorId, Color); }
		~ScopedColor() { ImGui::PopStyleColor(); }
	};

	class ScopedFont
	{
	public:
		ScopedFont(const ScopedFont&) = delete;
		ScopedFont operator=(const ScopedFont&) = delete;
		ScopedFont(ImFont* font) { ImGui::PushFont(font); }
		~ScopedFont() { ImGui::PopFont(); }
	};

	class ScopedID
	{
	public:
		ScopedID(const ScopedID&) = delete;
		ScopedID operator=(const ScopedID&) = delete;
		template<typename T>
		ScopedID(T id) { ImGui::PushID(id); }
		~ScopedID() { ImGui::PopID(); }
	};

	class ScopedColorStack
	{
	public:
		ScopedColorStack(const ScopedColorStack&) = delete;
		ScopedColorStack operator=(const ScopedColorStack&) = delete;

		template <typename ColorType, typename... OtherColors>
		ScopedColorStack(ImGuiCol firstColorID, ColorType firstColor, OtherColors&& ... otherColorPairs)
			: m_Count((sizeof... (otherColorPairs) / 2) + 1)
		{
			static_assert ((sizeof... (otherColorPairs) & 1u) == 0,
				"ScopedColorStack constructor expects a list of pairs of Color IDs and Colors as its arguments");

			PushColor(firstColorID, firstColor, std::forward<OtherColors>(otherColorPairs)...);
		}

		~ScopedColorStack() { ImGui::PopStyleColor(m_Count); }

	private:
		int m_Count;

		template <typename ColorType, typename... OtherColors>
		void PushColor(ImGuiCol ColorID, ColorType Color, OtherColors&& ... otherColorPairs)
		{
			if constexpr (sizeof... (otherColorPairs) == 0)
			{
				ImGui::PushStyleColor(ColorID, Color);
			}
			else
			{
				ImGui::PushStyleColor(ColorID, Color);
				PushColor(std::forward<OtherColors>(otherColorPairs)...);
			}
		}
	};

	class ScopedStyleStack
	{
	public:
		ScopedStyleStack(const ScopedStyleStack&) = delete;
		ScopedStyleStack operator=(const ScopedStyleStack&) = delete;

		template <typename ValueType, typename... OtherStylePairs>
		ScopedStyleStack(ImGuiStyleVar firstStyleVar, ValueType firstValue, OtherStylePairs&& ... otherStylePairs)
			: m_Count((sizeof... (otherStylePairs) / 2) + 1)
		{
			static_assert ((sizeof... (otherStylePairs) & 1u) == 0,
				"ScopedStyleStack constructor expects a list of pairs of Color IDs and Colors as its arguments");

			PushStyle(firstStyleVar, firstValue, std::forward<OtherStylePairs>(otherStylePairs)...);
		}

		~ScopedStyleStack() { ImGui::PopStyleVar(m_Count); }

	private:
		int m_Count;

		template <typename ValueType, typename... OtherStylePairs>
		void PushStyle(ImGuiStyleVar styleVar, ValueType value, OtherStylePairs&& ... otherStylePairs)
		{
			if constexpr (sizeof... (otherStylePairs) == 0)
			{
				ImGui::PushStyleVar(styleVar, value);
			}
			else
			{
				ImGui::PushStyleVar(styleVar, value);
				PushStyle(std::forward<OtherStylePairs>(otherStylePairs)...);
			}
		}
	};

	class ScopedItemFlags
	{
	public:
		ScopedItemFlags(const ScopedItemFlags&) = delete;
		ScopedItemFlags operator=(const ScopedItemFlags&) = delete;
		ScopedItemFlags(const ImGuiItemFlags flags, const bool enable = true) { ImGui::PushItemFlag(flags, enable); }
		~ScopedItemFlags() { ImGui::PopItemFlag(); }
	};

	void DrawBorder(ImRect rect, float thickness = 1.0f, float rounding = 0.0f, float offsetX = 0.0f, float offsetY = 0.0f);
}
