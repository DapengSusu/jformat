#ifndef FORMAT_H
#define FORMAT_H

#include <sstream>

#include "fmt.h"

namespace jumper {

// 内部命名空间 jumper_inner
namespace jumper_inner {
template<typename T>
std::ostringstream& __format(std::ostringstream& oss,
    std::deque<std::string>& subs, const T& t)
{
    if (subs.empty())
    {
        return oss;
    }

    oss << subs.front();
    subs.pop_front();

    // 只剩尾串，忽略最后一个多余参数
    if (subs.empty())
    {
        return oss;
    }

    oss << t;

    for (auto& str: subs)
    {
        oss << str;
    }

    return oss;
}

template<typename T, typename... Args>
std::ostringstream& __format(std::ostringstream& oss,
    std::deque<std::string>& subs, const T& t, const Args&... args)
{
    if (subs.empty())
    {
        return oss;
    }

    oss << subs.front();
    subs.pop_front();

    // 只剩尾串，忽略多余参数
    if (subs.empty())
    {
        return oss;
    }

    oss << t;

    return __format(oss, subs, args...);
}

template<typename T, typename... Args>
std::string _format(Fmt& fmt, const T& t, const Args&... args)
{
    if (!fmt.is_ok() || fmt.to_substrings().size()-1 > sizeof...(args)+1)
    {
        return std::string();
    }
    std::ostringstream oss;

    return __format(oss, fmt.to_substrings(), t, args...).str();
}
} // namespace jumper_inner

/// 直接接受一个Fmt对象的可变引用，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
std::string format(Fmt& fmt, const T& t, const Args&... args)
{
    return jumper_inner::_format(fmt, t, args...);
}

/// 接受C字符串并构造Fmt对象，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
std::string format(const char* fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return jumper_inner::_format(fmt, t, args...);
}

/// 接受string并构造Fmt对象，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
std::string format(const std::string& fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return jumper_inner::_format(fmt, t, args...);
}

} // namespace jumper

#endif // FORMAT_H
