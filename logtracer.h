#include <fstream>
#include <chrono>
#include <mutex>
#include <map>

#include "format.h"

namespace jumper {

/// log级别，大于等于当前log级别才能显示
enum class LogLevel: int {
    LOG_DEBUG = 1,
    LOG_INFO = 2,
    LOG_WARNING = 3,
    LOG_ERROR = 4,
};

// log级别的快速定义
const LogLevel LV_DEBUG = LogLevel::LOG_DEBUG;
const LogLevel LV_INFO = LogLevel::LOG_INFO;
const LogLevel LV_WARNING = LogLevel::LOG_WARNING;
const LogLevel LV_ERROR = LogLevel::LOG_ERROR;

// 内部命名空间 jumper_inner
namespace jumper_inner {
// log颜色表
const std::map<LogLevel, std::pair<std::string, std::string>> logHeaderMap {
    { LV_DEBUG, { "\e[37m", "[DEBUG]:" } },          // 浅灰
    { LV_INFO, { "\e[32m", "[INFO]:" } },            // 绿色
    { LV_WARNING, { "\e[33m", "[WARNING]:" } },      // 黄色
    { LV_ERROR, { "\e[1;31m", "[ERROR]:" } },        // 红色（加粗）
};

// mutex锁
extern std::mutex g_inner_mutex;
} // namespace jumper_inner

class LogTracer {
public:
    /// 初始化LogTracer环境
    /// 设置log输出路径，并添加时间戳，设置log输出级别，默认Info级别
    /// 如果没有将log记录到文件的需要，可不调用此函数
    inline static void InitialTracer(LogLevel level = LV_INFO,
        const std::string& logPath = "./logtracer.txt")
    {
        std::lock_guard<std::mutex> lock(jumper_inner::g_inner_mutex);

        FinalTracer();
        s_ofs.open(logPath, std::ios_base::app);
        if (s_ofs.is_open())
        {
            s_ofs << "--------------------\n" << TimeStamp() << "\n";
        }
        else
        {
            std::cerr << "[logtracer]: can't open log file:" << logPath << "\n";
        }
    }

    /// 设置当前log输出等级，影响之后的log输出，之前的不受影响
    inline static void SetLogLevel(LogLevel level)
    {
        s_level = level;
    }

    /// 刷新log显示
    static void FlushTracer()
    {
        std::cout << std::flush;
    }

    /// 关闭LogTracer
    static void FinalTracer()
    {
        FlushTracer();
        if (s_ofs.is_open())
        {
            s_ofs.close();
        }
    }

    /// 获取当前时间戳
    static std::string TimeStamp()
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time_t);

        char buffer[128];
        ::memset(buffer, '\0', sizeof(buffer));
        ::strftime(buffer, sizeof(buffer), "%F %T", now_tm);

        return std::string(buffer);
    }

    /// Debug级别log输出，不带换行符
    template<typename... Args>
    inline static void LogDebug(const std::string& log, const Args&... args)
    {
        print(std::cout, LV_DEBUG, log, args...);
    }

    /// Debug级别log输出，自带换行符
    template<typename... Args>
    inline static void LoglnDebug(const std::string& log, const Args&... args)
    {
        println(std::cout, LV_DEBUG, log, args...);
    }

    /// Info级别log输出，不带换行符
    template<typename... Args>
    inline static void LogInfo(const std::string& log, const Args&... args)
    {
        print(std::cout, LV_INFO, log, args...);
    }

    /// Info级别log输出，自带换行符
    template<typename... Args>
    inline static void LoglnInfo(const std::string& log, const Args&... args)
    {
        println(std::cout, LV_INFO, log, args...);
    }

    /// Warning级别log输出，不带换行符
    template<typename... Args>
    inline static void LogWarning(const std::string& log, const Args&... args)
    {
        print(std::cout, LV_WARNING, log, args...);
    }

    /// Warning级别log输出，自带换行符
    template<typename... Args>
    inline static void LoglnWarning(const std::string& log, const Args&... args)
    {
        println(std::cout, LV_WARNING, log, args...);
    }

    /// Error级别log输出，不带换行符
    template<typename... Args>
    inline static void LogError(const std::string& log, const Args&... args)
    {
        print(std::cerr, LV_ERROR, log, args...);
    }

    /// Error级别log输出，自带换行符
    template<typename... Args>
    inline static void LoglnError(const std::string& log, const Args&... args)
    {
        println(std::cerr, LV_ERROR, log, args...);
    }

private:
    // 此log级别是否能显示，只有大于设置的log级别才能显示
    inline static bool is_show(LogLevel level)
    {
        return static_cast<int>(level) >= static_cast<int>(s_level);
    }

    // 此log级别对应的log颜色
    inline static const std::string& log_color(LogLevel level)
    {
        auto iter = jumper_inner::logHeaderMap.find(level);

        return iter->second.first;
    }

    // 此log级别对应的log头部
    inline static const std::string& log_header(LogLevel level)
    {
        auto iter = jumper_inner::logHeaderMap.find(level);

        return iter->second.second;
    }

    // 输出log，不带换行符
    template<typename T, typename... Args>
    inline static std::ostream& print(std::ostream& os, LogLevel lv,
        const std::string& log, const T& t, const Args&... args)
    {
        if (!is_show(lv))
        {
            return os;
        }

        auto color(log_color(lv));
        auto header(log_header(lv));
        auto body(jumper::format(log, t, args...));
        std::lock_guard<std::mutex> lock(jumper_inner::g_inner_mutex);

        if (s_ofs.is_open())
        {
            s_ofs << header << body << "\n";
        }

        return (os << color << header << body << "\e[0m");
    }

    // 输出log，不带换行符
    inline static std::ostream& print(std::ostream& os,
        LogLevel lv, const std::string& log)
    {
        if (!is_show(lv))
        {
            return os;
        }

        auto color(log_color(lv));
        auto header(log_header(lv));
        std::lock_guard<std::mutex> lock(jumper_inner::g_inner_mutex);

        if (s_ofs.is_open())
        {
            s_ofs << header << log << "\n";
        }

        return (os << color << header << log << "\e[0m");
    }

    // 输出log，自带换行符
    template<typename T, typename... Args>
    inline static std::ostream& println(std::ostream& os, LogLevel lv,
        const std::string& log, const T& t, const Args&... args)
    {
        if (!is_show(lv))
        {
            return os;
        }

        return (print(os, lv, log, t, args...) << "\n");
    }

    // 输出log，自带换行符
    inline static std::ostream& println(std::ostream& os,
        LogLevel lv, const std::string& log)
    {
        if (!is_show(lv))
        {
            return os;
        }

        return print(os, lv, log) << "\n";
    }

private:
    // 当前环境中的log级别，默认Info
    static LogLevel s_level;

    // log文件输出流
    static std::ofstream s_ofs;
};

} // namespace jumper
