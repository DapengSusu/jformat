#include <cstring>
#include <chrono>

#include "logtracer.h"

jumper::LogLevel jumper::LogTracer::s_level = jumper::LV_INFO;
std::ofstream jumper::LogTracer::s_ofs;
std::mutex jumper::LogTracer::s_mutex;

/// 初始化LogTracer环境
/// 设置log输出路径，并添加时间戳，设置log输出级别，默认Info级别
/// 如果没有将log记录到文件的需要，可不调用此函数
void jumper::LogTracer::InitialTracer(LogLevel level, const std::string& logPath)
{
    std::lock_guard<std::mutex> lock(s_mutex);

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

/// 获取当前时间戳，精度秒
std::string jumper::LogTracer::TimeStamp()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    char buffer[128];
    ::memset(buffer, '\0', sizeof(buffer));
    ::strftime(buffer, sizeof(buffer), "%F %T", now_tm);

    return std::string(buffer);
}
