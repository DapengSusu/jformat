#ifndef FORMAT_H
#define FORMAT_H

#include <sstream>

#include "fmt.h"

namespace jumper {

// 内部命名空间inner，外部请勿使用
namespace inner {
template<typename T>
std::ostringstream& __format(std::ostringstream& oss,
    std::deque<std::string>& subs, const T& t)
{
    if (!subs.empty())
    {
        oss << subs.front() << t;
        subs.pop_front();
        
        for (auto& str: subs)
        {
            oss << str;
        }
    }

    return oss;
}

template<typename T, typename... Args>
std::ostringstream& __format(std::ostringstream& oss,
    std::deque<std::string>& subs, const T& t, const Args&... args)
{
    auto size = sizeof...(args) + 1;
    if (subs.empty() || subs.size()-1 > size)
    {
        return oss;
    }

    oss << subs.front() << t;
    subs.pop_front();

    return __format(oss, subs, args...);
}

template<typename T, typename... Args>
std::string _format(Fmt& fmt, const T& t, const Args&... args)
{
    if (!fmt.is_ok())
    {
        return std::string();
    }
    std::ostringstream oss;

    return __format(oss, fmt.to_substrings(), t, args...).str();
}
} // namespace inner

/// 直接接受一个Fmt对象的可变引用，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
std::string format(Fmt& fmt, const T& t, const Args&... args)
{
    return inner::_format(fmt, t, args...);
}

/// 接受C字符串并构造Fmt对象，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
std::string format(const char* fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return inner::_format(fmt, t, args...);
}

/// 接受string并构造Fmt对象，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
std::string format(const std::string& fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return inner::_format(fmt, t, args...);
}

} // namespace jumper

#endif // FORMAT_H
