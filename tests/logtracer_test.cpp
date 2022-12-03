#include <future>
#include <vector>

#include "logtracer.h"

using jumper::LogTracer;

int main()
{
    // 设置当前环境log输出级别，并指定log文件保存路径。如果不需要log文件，可省略
    // LogTracer::InitialTracer(jumper::LV_DEBUG, "./logtracer.txt");

    // 也可以单独设置log输出级别，只有log不低于设置的级别才能输出显示
    // LV_DEBUG = 1,
    // LV_INFO = 2,
    // LV_WARNING = 3,
    // LV_ERROR = 4,
    LogTracer::SetLogLevel(jumper::LV_DEBUG);

    // 只有重载了<<运算符的对象才能直接作为输出参数
    LogTracer::LoglnDebug("This is a debug message.");
    LogTracer::LoglnInfo("sum:{} + {} = {}", 1, 1, 1+1);
    LogTracer::LoglnWarning("{} has been skiped!", "Ann");
    LogTracer::LoglnError("Run failed!");

    // 立即刷新log缓冲，在需要之前的log立即输出时添加。
    // 为了提高性能，除了Error级别的log（cerr实现）会立即刷新缓冲区；
    // 其它级别的log（cout实现）默认都不会立即刷新，会在缓冲区满了之后刷新；
    // 如果出现log输出显示不及时，或者log输出位置与预期不符，可以添加这句调用排查是否缓冲没有及时刷新。
    // 注意，大多数时候都不需要我们手动刷新，尽量避免频繁的刷新缓冲（例如在循环中），可能会导致性能问题。
    // LogTracer::FlushTracer(); // 立即刷新缓冲
    // LogTracer::FlushlnTracer(); // 立即刷新缓冲并追加换行符

    // 格式语句中，必须是成对匹配的"{}"，其中间的内容将被忽略
    LogTracer::LoglnWarning("tips: {ignored xxx}", "something was ignored...");

    // 错误！格式符不匹配，这将会返回一个空字符串！
    // LogTracer::LoglnError("No matched bracket:{}, then }", '{');

    // 注意：允许参数比格式符多，多余的参数将被忽略
    LogTracer::LoglnInfo("I'm {}.", "Jumper", 24);
    LogTracer::LoglnInfo("one:{}, two:{}, three:", 1, 2, 3, 4, 5);

    // 错误！不允许格式符比参数多，这将会返回一个空字符串！
    // LogTracer::LoglnInfo("{} + {} = {}", 5);

    // 不带ln的版本，不会自动插入换行符'\n'，需要自己控制格式，否则log会挤在一行
    LogTracer::LogDebug("You have to control the foramt yourself!\n");
    LogTracer::LogDebug("The next line\n");
    
    // 改变log输出级别，这将会影响之后的log输出，现在级别低于Info的log（Debug级别的log）将不再输出！
    LogTracer::SetLogLevel(jumper::LV_INFO);

    // log级别低于当前设置的log级别，将不再输出显示（也不会被写入文件记录！）
    LogTracer::LoglnDebug("You can NOT see me! hahaha");

    // 立即关闭log文件，之后的log将不会写入文件，但仍然会在终端输出。
    // 可选，默认在程序结束时自动关闭
    // LogTracer::FinalTracer();

    return 0;
}
