#ifndef FORMAT_H
#define FORMAT_H

#include <sstream>
#include <iostream>

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
inline std::string _format(const Fmt& fmt, const T& t, const Args&... args)
{
    if (!fmt.is_ok() || fmt.subs().size()-1 > sizeof...(args)+1)
    {
        return std::string();
    }
    std::ostringstream oss;
    std::deque<std::string> subs(fmt.subs());

    return __format(oss, subs, t, args...).str();
}
} // namespace jumper_inner

/// 直接接受一个Fmt对象的可变引用，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
inline std::string format(const Fmt& fmt, const T& t, const Args&... args)
{
    return jumper_inner::_format(fmt, t, args...);
}

/// 不带换行输出
template<typename T, typename... Args>
inline std::ostream& print(const Fmt& fmt, const T& t, const Args&... args)
{
    return (std::cout << format(fmt, t, args...));
}

inline std::ostream& print(const Fmt& fmt)
{
    return (std::cout << fmt.to_str());
}

/// 带换行输出
template<typename T, typename... Args>
inline std::ostream& println(const Fmt& fmt, const T& t, const Args&... args)
{
    return (print(fmt, t, args...) << "\n");
}

inline std::ostream& println(const Fmt& fmt)
{
    return (print(fmt) << "\n");
}

/// 接受C字符串并构造Fmt对象，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
inline std::string format(const char* fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return jumper_inner::_format(fmt, t, args...);
}

/// 不带换行输出
template<typename T, typename... Args>
inline std::ostream& print(const char* fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return print(fmt, t, args...);
}

inline std::ostream& print(const char* fmtStr)
{
    Fmt fmt(fmtStr);

    return print(fmt);
}

/// 带换行输出
template<typename T, typename... Args>
inline std::ostream& println(const char* fmtStr, const T& t, const Args&... args)
{
    return (print(fmtStr, t, args...) << "\n");
}

inline std::ostream& println(const char* fmtStr)
{
    return (print(fmtStr) << "\n");
}

/// 接受string并构造Fmt对象，如果Fmt对象无效，返回空字符串
template<typename T, typename... Args>
inline std::string format(const std::string& fmtStr, const T& t, const Args&... args)
{
    Fmt fmt(fmtStr);

    return jumper_inner::_format(fmt, t, args...);
}

/// 不带换行输出
template<typename T, typename... Args>
inline std::ostream& print(const std::string& fmtStr, const T& t, const Args&... args)
{
    return print(fmtStr.c_str(), t, args...);
}

inline std::ostream& print(const std::string& fmtStr)
{
    return print(fmtStr.c_str());
}

/// 带换行输出
template<typename T, typename... Args>
inline std::ostream& println(const std::string& fmtStr, const T& t, const Args&... args)
{
    return println(fmtStr.c_str(), t, args...);
}

inline std::ostream& println(const std::string& fmtStr)
{
    return println(fmtStr.c_str());
}

} // namespace jumper

#endif // FORMAT_H
