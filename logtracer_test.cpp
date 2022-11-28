#include "logtracer.h"

#include <future>
#include <vector>

using jumper::LogTracer;

// 性能压测
void performance(int thread = 1, int round = 10, int count = 10000);

int main()
{
    // 如果不需要log文件，可省略
    // LogTracer::InitialTracer();
    // 设置log输出级别为Debug
    LogTracer::SetLogLevel(jumper::LV_DEBUG);

    // 只有重载了<<运算符的对象才能直接作为输出参数
    LogTracer::LoglnDebug("This is a debug message.");
    LogTracer::LoglnInfo("sum:{} + {} = {}", 1, 1, 1+1);
    LogTracer::LoglnWarning("{} has been skiped!", "Ann");
    LogTracer::LoglnError("Run failed!");

    // 格式语句中，必须是成对匹配的"{}"，其中间的内容将被忽略
    LogTracer::LoglnWarning("tips: {ignored xxx}", "something was ignored...");

    // 错误！格式符不匹配，这将会返回一个空字符串！
    // LogTracer::LoglnError("No matched bracket:{}, then }", '{');

    // 注意：参数比格式多是允许的，多余的参数将被忽略
    LogTracer::LoglnInfo("I'm {}.", "Jumper", 24);
    LogTracer::LoglnInfo("one:{}, two:{}, three:", 1, 2, 3, 4, 5);

    // 错误！不允许格式付符比参数多，这将会返回一个空字符串！
    // LogTracer::LoglnInfo("{} + {} = {}", 5);

    // 不带ln的版本，不会自动插入换行符'\n'，需要自己控制格式，否则log会挤在一行
    LogTracer::LogDebug("You have to control the foramt yourself!\n");
    LogTracer::LogDebug("The next line\n");
    
    // 改变log输出级别
    LogTracer::SetLogLevel(jumper::LV_INFO);

    // log级别低于当前设置的log级别，将不再输出显示（也不会被写入文件记录！）
    LogTracer::LoglnDebug("You can NOT see me! hahaha");

    // 性能压测
    // 不能多个线程往同一个文件写入log
    // performance(1);

    // 刷新log，可视情况添加
    // LogTracer::FlushTracer();

    // 关闭log文件，可选，默认在程序结束时自动关闭
    // LogTracer::FinalTracer();

    return 0;
}

// 性能压测
void performance(int thread, int round, int count)
{
    std::vector<std::future<void>> futVec;
    for (auto t = 0; t != thread; ++t)
    {
        auto fut = std::async(std::launch::async, [=]() {
            for (auto i = 0; i != round; ++i)
            {
                for (auto j = 0; j != count; ++j)
                {
                    LogTracer::LoglnDebug("dahds{}hdahdodkadaak{}dhakdh", "+", "-");
                    LogTracer::LoglnError("dahds{}hdahhdhak{}dhakdh", "+", "-");
                    LogTracer::LoglnWarning("dahds{}hdahdodkadahdhak{}dhakdh", "+", "-");
                    LogTracer::LoglnDebug("dahds{}hdahdodkadahdhak{}dhakdh", "+", "-");
                    LogTracer::LoglnError("dahds{}hdahdhakdh", "+");
                    LogTracer::LoglnWarning("dahds{}hdahdodkadahdhak{}dhakdh", "+", "-");
                    LogTracer::LoglnInfo("dahddodkadahdhak{}dhakdh", "-");
                }
            }
        });
        futVec.emplace_back(std::move(fut));
    }

    for (auto &fut: futVec)
    {
        fut.wait();
    }
}
