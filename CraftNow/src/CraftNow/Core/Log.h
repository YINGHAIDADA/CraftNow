#pragma once

#include "CraftNow/Core/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace CraftNow {

	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	};

}

// Core Log macros
#define CN_CORE_TRACE(...)   ::CraftNow::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CN_CORE_INFO(...)	 ::CraftNow::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CN_CORE_WARN(...)	 ::CraftNow::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CN_CORE_ERROR(...)	 ::CraftNow::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CN_CORE_FATAL(...)   ::CraftNow::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log macros
#define CN_TRACE(...)        ::CraftNow::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CN_INFO(...)	     ::CraftNow::Log::GetClientLogger()->info(__VA_ARGS__)
#define CN_WARN(...)	     ::CraftNow::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CN_ERROR(...)	     ::CraftNow::Log::GetClientLogger()->error(__VA_ARGS__)
#define CN_FATAL(...)        ::CraftNow::Log::GetClientLogger()->critical(__VA_ARGS__)
