#pragma once
#if _WIN32
#ifdef __PPROCESS_TOOLS__
#define PPROCESS_TOOLS_API __declspec(dllexport)
#else
#define PPROCESS_TOOLS_API __declspec(dllimport)
#endif
#else
#define PPROCESS_TOOLS_API
#endif

#define _PPROCESS_TOOLS_BEGIN_ \
    namespace PPROCESS_TOOLS   \
    {
#define _PPROCESS_TOOLS_END_ }