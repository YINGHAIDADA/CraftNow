#pragma once

#include "CraftNow/Core/Core.h"
#include "CraftNow/Core/Log.h"
#include <filesystem>

#ifdef CN_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define CN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { CN##type##ERROR(msg, __VA_ARGS__); CN_DEBUGBREAK(); } }
#define CN_INTERNAL_ASSERT_WITH_MSG(type, check, ...) CN_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define CN_INTERNAL_ASSERT_NO_MSG(type, check) CN_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", CN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define CN_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define CN_INTERNAL_ASSERT_GET_MACRO(...) CN_EXPAND_MACRO( CN_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, CN_INTERNAL_ASSERT_WITH_MSG, CN_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define CN_ASSERT(...) CN_EXPAND_MACRO( CN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define CN_CORE_ASSERT(...) CN_EXPAND_MACRO( CN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define CN_ASSERT(...)
#define CN_CORE_ASSERT(...)
#endif