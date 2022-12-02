#ifndef FMT_H
#define FMT_H

#include <iostream>
#include <string>
#include <deque>
#include <stack>

// Debug宏开关，默认关闭
#ifndef JDEBUG
// #define JDEBUG
#endif

namespace jumper {

// 将变量名转换成相应的字符串，变量必须已定义
#define TO_STRING(x) (x,#x)

using std::string;
using std::deque;
using std::stack;
using std::pair;

#ifdef JDEBUG
using std::cout;
using std::cerr;
using std::endl;
#endif

// string的pos
using StrPos = string::size_type;

/// 移除字符串首尾的空字符
inline string&& trim(const string& str)
{
    auto begin = str.find_first_not_of(' ');
    auto end = str.find_last_not_of(' ');
    string s(str, begin, end - begin + 1);

    return std::move(s);
}

/// 移除字符串头部的空字符
inline string&& trim_front(const string& str)
{
    auto begin = str.find_first_not_of(' ');
    string s(str, begin);

    return std::move(s);
}

/// 移除字符串尾部的空字符
inline string&& trim_back(const string& str)
{
    auto end = str.find_last_not_of(' ');
    string s(str, 0, end + 1);

    return std::move(s);
}

/**
  * @brief Fmt创建格式化字符串所需的格式，以"{}"包含每个需要格式化的参数
  * @note 转义'{'请使用"{{"，转义'}'请使用"}}"，最终不会被格式化，而是输出一个'{'或者'}'
  * @note Fmt对象不允许拷贝，只能移动，请使用移动语义(std::move(fmt))
*/
class Fmt {
public:
    Fmt() = default;

    /// 接受一个string对象作为格式构造Fmt，不允许隐式类型转换
    explicit Fmt(const string& fmt)
        : m_fmt(fmt), m_buf(fmt.length(), '\0')
    {
        dispose();
    }

    /// 接受一个C字符串作为格式构造Fmt（委托构造）
    explicit Fmt(const char* fmt) : Fmt(string(fmt)) {}

    // Fmt对象不允许拷贝
    Fmt(const Fmt&) = delete;
    Fmt& operator=(const Fmt&) = delete;

    // Fmt对象可以移动
    Fmt(Fmt&& fmt) noexcept : m_status(fmt.m_status),
        m_fmt(std::move(fmt.m_fmt)), m_buf(std::move(fmt.m_buf)),
        m_subStrDeque(std::move(fmt.m_subStrDeque))
    {
        fmt.m_status = false;
        fmt.m_buf.clear();
        fmt.m_fmt.clear();
        fmt.m_subStrDeque.clear();
    }

    Fmt& operator=(Fmt&& fmt) noexcept
    {
        std::move(fmt).swap(*this);

        fmt.m_status = false;
        fmt.m_buf.clear();
        fmt.m_fmt.clear();
        fmt.m_subStrDeque.clear();

        return *this;
    }

    // Fmt对象支持使用+追加格式
    Fmt& operator+(const Fmt& fmt)
    {
        return *this + fmt.m_fmt;
    }

    // Fmt对象支持使用+=追加格式
    Fmt& operator+=(const Fmt& fmt)
    {
        return *this + fmt.m_fmt;
    }

    
    // Fmt对象支持使用+追加格式
    Fmt& operator+(const char* fmt)
    {
        return *this + string(fmt);
    }

    // Fmt对象支持使用+=追加格式
    Fmt& operator+=(const char* fmt)
    {
        return *this + string(fmt);
    }

    // Fmt对象支持使用+追加格式
    Fmt& operator+(const std::string& fmt)
    {
        m_fmt += fmt;
        m_buf.resize(m_fmt.length(), '\0');
        dispose();

        return *this;
    }

    // Fmt对象支持使用+=追加格式
    Fmt& operator+=(const std::string& fmt)
    {
        m_fmt += fmt;
        m_buf.resize(m_fmt.length(), '\0');
        dispose();

        return *this;
    }

    /// 重新设置当前Fmt对象中的格式
    inline Fmt& set_fmt(string&& fmt)
    {
        m_status = true;
        m_fmt = std::move(fmt);
        fmt.clear();
        m_buf.resize(m_fmt.length(), '\0');

        dispose();

        return *this;
    }

    inline Fmt& set_fmt(const char* fmt)
    {
        return set_fmt(std::string(fmt));
    }

    /// 获取Fmt对象中的格式，如果Fmt中的格式解析失败了，返回空串
    inline const string& to_str() const
    {
        return m_fmt;
    }

    /// Fmt对象中的格式解析成功了，返回true，使用Fmt对象前请务必确认
    inline bool is_ok() const
    {
        return m_status;
    }

    /// 格式串解析成功后，从'{'和'}'处进行切割，生成的子串按序存储到队列中
    /// 返回存放子串队列的可变引用
    inline deque<string>& to_substrings()
    {
        return m_subStrDeque;
    }

    /// 常函数版本，返回存放子串队列的不可变引用
    inline const deque<string>& to_substrings() const
    {
        return m_subStrDeque;
    }

    // 调试使用，Fmt对象中格式解析失败时，buf中会保存成功的部分串
    inline const string& buf() const
    {
        return m_buf;
    }
    
private:
    // 交换两个Fmt对象
    inline void swap(Fmt& fmt)
    {
        using std::swap;

        swap(this->m_status, fmt.m_status);
        swap(this->m_fmt, fmt.m_fmt);
        swap(this->m_buf, fmt.m_buf);
        swap(this->m_subStrDeque, fmt.m_subStrDeque);
    }

    // 在构造Fmt对象和重新设置格式时自动调用. 解析Fmt中的格式，失败会设置status为false
    // 解析规则："{{"和"}}"被视为转义，在格式串中对应一个'{'或者'}';
    // '{'和'}'必须配对，一个'{'必须对应一个'}'，且'}'不能出现在'{'左侧;
    // 从所有解析的'{'和'}'处，将整个格式串切割成数个子串保存;
    // 一对成功配对的'{'和'}'之间的内容也会被保留，并被切割为一个子串存储.
    void dispose()
    {
        m_status = true;
        m_buf.clear();
        m_subStrDeque.clear();

        // 没有格式符，不需要格式化
        if (string::npos == m_fmt.find('{') && string::npos == m_fmt.find('}'))
        {
            m_subStrDeque.emplace_back(m_fmt);

            return;
        }

        StrPos lastPos = 0;
        stack<StrPos> lPosBrackets;
        deque<pair<StrPos, StrPos>> brackets;
        // 逐字符遍历
        for (auto iter = m_fmt.cbegin(); iter != m_fmt.cend(); ++iter)
        {
            ++lastPos;
            m_buf.push_back(*iter);
            if ('{' == *iter)
            {
                // 格式串末尾出现'{'，肯定无法配对
                if (m_fmt.cend() == iter + 1)
                {
#ifdef JDEBUG
                    cerr << "[fmt]Error: no matched '}" << endl;
#endif // JDEBUG
                    m_status = false;
                    break;
                }
                if ('{' == *(iter + 1))
                {
                    // 如果连续两个'{'，则此'{'作为转义输出，不参与配对
                    ++iter;
                    continue;
                }

                // 待配对的'{'
                lPosBrackets.push(lastPos - 1);
            }

            if ('}' == *iter)
            {
                // 连续两个'}'，则此'}'作为转义输出，不参与配对
                if (m_fmt.cend() != iter + 1 && '}' == *(iter + 1))
                {
                    ++iter;
                    continue;
                }
                // 没有'{'待匹配，多余的'}'，匹配失败
                if (lPosBrackets.empty())
                {
#ifdef JDEBUG
                    cerr << "[fmt]Error: no matched '{" << endl;
#endif // JDEBUG
                    m_status = false;
                    break;
                }

                // 匹配成功，将'{'和'}'的pos保存
                brackets.emplace_back(lPosBrackets.top(), lastPos - 1);
                lPosBrackets.pop();
            }
        }

        // 还有待匹配的'{'，多余的'{'，匹配失败
        if (!lPosBrackets.empty())
        {
#ifdef JDEBUG
            cerr << "[fmt]Error: no enough matched '}" << endl;
#endif // JDEBUG
            m_status = false;
        }

        // 匹配成功，按格式规则切割，保存子串
        if (m_status)
        {
            m_fmt = std::move(m_buf);
            m_buf.clear();

            StrPos pos = 0;
            for (auto &splitPos: brackets)
            {
                string subStr(m_fmt, pos, splitPos.first - pos);

                m_subStrDeque.emplace_back(std::move(subStr));
                pos = splitPos.second + 1;
            }
            m_subStrDeque.emplace_back(m_fmt.substr(pos));
        }
        else
        {
            // 匹配失败清空格式串
            m_fmt.clear();
        }
    }

    // 记录Fmt对象解析是否成功
    bool m_status = true;
    // 存储格式字符串，匹配失败时清空
    string m_fmt;
    // 临时格式字符串，匹配成功时清空
    string m_buf;
    // 子串队列，按序尾插
    deque<string> m_subStrDeque;
};

/// 工具函数，测试当前Fmt对象是否解析成功
inline bool check_fmt(const Fmt& fmt)
{
    auto bRet = fmt.is_ok();
    if (bRet)
    {
        std::cout << "[ok]:" << fmt.to_str() << "\n|";
        for (const auto &str: fmt.to_substrings())
        {
            std::cout << str << "|";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cerr << "[ng]:" << fmt.buf() << std::endl;
    }

    return bRet;
}

/// 工具函数，测试当前格式字符串构造Fmt对象是否能解析成功
inline bool check_fmt(const std::string& s)
{
    Fmt fmt(s);

    return check_fmt(fmt);
}

} // namespace jumper

#endif // FMT_H
