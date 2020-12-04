#pragma once

#include <Windows.h>
#include <debugapi.h>

#ifdef max
#undef max
#endif

#include <sstream>

#define LOG(message)                       \
  {                                        \
    std::wostringstream os_;               \
    os_ << message << std::endl;           \
    OutputDebugStringW(os_.str().c_str()); \
  }

#if _DEBUG

#include <string>

#define RTIDPRR_ASSERT_MSG(condition, message)                             \
  {                                                                        \
    if (!(condition)) {                                                    \
      std::string fileInfo = message;                                      \
      std::wstring wInfo = std::wstring(fileInfo.begin(), fileInfo.end()); \
      LOG("In file: " << __FILE__ << ", line: " << __LINE__                \
                      << " of function: " << __FUNCTION__                  \
                      << "Condition failed : " << #condition << std::endl  \
                      << message << std::endl);                            \
      __debugbreak();                                                      \
    }                                                                      \
  }
#else
#define RTIDPRR_ASSERT_MSG(condition, message) (condition)
#endif

#define RTIDPRR_ASSERT(condition) RTIDPRR_ASSERT_MSG(condition, "")
