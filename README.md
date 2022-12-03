### 字符串格式化 jumper::format(...)函数

#### 用法

`format` 函数依赖 `Fmt` 对象提供基础功能，首先介绍下 `Fmt` 类:

> `Fmt` 解析指定格式生成格式串，它根据配对的 __'{'__ 和 __'}'__ 分割格式串，使用 `"{{"` 以及 `"}}"` 作为转义符。
>
> `Fmt` 有两种构造方式：
>
> 1. 直接指定一个格式字符串，`Fmt fmt("a = {}");`
> 2. 先构造空对象，再设置格式，`Fmt fmt;fmt.set_fmt("a = {}");`
>
> 事实上，`Fmt` 对象随时都可以调用 `set_fmt` 修改格式，也可以使用 __+__ 或者 __+=__ 拼接两个 `Fmt` 对象或者直接拼接一个带有格式符的字符串。
>
> __在构造 `Fmt` 对象之后，可以调用 `is_ok()` 来检查格式串是否解析成功，解析失败 `Fmt` 对象将持有一个空的格式串，后续的操作也就无从谈起了。__

我们可以通过创建一个 `Fmt` 对象来包含我们需要的格式，然后作为参数传递给 `format` ，更常见的情况是，我们直接使用字符串作为格式参数：

```c++
std::string str(jumper::format("{} + {} = {}", 1, 1, 1+1));

EXPECT_EQ(str, "1 + 1 = 2"); // 这是Googletest框架的check宏
```

当然也可以：

```c++
Fmt fmt("5 - {}");

fmt += " = {}";
std::string str(jumper::format(fmt, 2, 5-2));

EXPECT_EQ(str, "5 - 2 = 3");
```

传入的参数数量可以比格式符多，多余的参数将被忽略：

```c++
auto str(jumper::format("one is {}, two is {}.", 1, 2, 3, 4, 5));

EXPECT_EQ(str, "one is 1, two is 2.");
```

但是不允许格式符的数量比参数多，这将会返回一个空字符串：

```c++
auto str(jumper::format("one is {}, two is {}.", 1));

EXPECT_TRUE(str.empty()); // str is an empty string!
```

> __`format` 使用技巧__
>
> 因为 `format` 依赖 `Fmt` 的功能，每次传入 `string` 或者C字符串时，都会构造一个临时的 `Fmt` 对象，这个过程需要解析格式，识别格式符 `"{}"` 和转义符 `"{{"` 以及 `"}}"`，这是有一定开销的。所以，对同一种格式多次格式化，包括下面要提到的格式化输出函数和 `LogTracer` 库，同样的格式多次格式化输出（例如在循环中），我们可以提前构造一个 `Fmt` 对象，之后多次使用，这样就省去了反复构造相同对象的开销。
>
> ```c++
> int a = 0;
> int b = 0;
> Fmt fmt("loop index: {}, a = {}, b = {}");
> ASSERT_TRUE(fmt.is_ok()); // Fmt对象必须有效，使用前需要判断
> for (auto i = 0; i != 10; ++i)
> {
>     ++a;
>     b += 2;
> 
>     jumper::println(fmt, i, a, b);
> }
> 
> EXPECT_EQ(a, 10);
> EXPECT_EQ(b, 20);
> ```
>
> 输出如下：
>
> ```shell
> loop index: 0, a = 1, b = 2
> loop index: 1, a = 2, b = 4
> loop index: 2, a = 3, b = 6
> loop index: 3, a = 4, b = 8
> loop index: 4, a = 5, b = 10
> loop index: 5, a = 6, b = 12
> loop index: 6, a = 7, b = 14
> loop index: 7, a = 8, b = 16
> loop index: 8, a = 9, b = 18
> loop index: 9, a = 10, b = 20
> ```

`format` 库中还同提供了两个格式化输出的函数：`print(...)` 和 `println(...)`，它们有多个版本的重载函数，可传入C++ `string` 对象或者C字符串或者 `Fmt` 对象，带 `ln` 的版本会自动追加一个换行符 `'\n'`。

```c++
// 直接传入C字符串
jumper::println("Input something...");
jumper::print("A -> {}\n", 'a');

// 传入string对象
jumper::println(std::string("d -> {}"), 'D');

// 传入Fmt对象
Fmt fmt("{} is my best friend.\n");
jumper::print(fmt, "Anna");
```

__<span style="color:red"> 注意，只有重载了 << 运算符的对象才可以直接作为 `format` 函数的参数，否则 `format` 将不知道以何种格式输出此对象。</span>__

另外，建议使用限定作用域的方式来使用 `format`，即 `jumper::format(...)`，而不是这样：

```c++
using jumper::format;

std::string str(format(...));
```

因为 `C++20` 标准新增了 `<format>` [库][format]，其中也有 `std::format` 函数，但标准库的功能更加强大，大部分时候 `std` 会被导入当前作用域，为了防止潜在的命名冲突，也为了兼容性，建议带作用域调用 `format`。

目前 `C++20` 推广使用还需要一段时间，且各家编译器对 `<format>` 支持还不够，可以暂时使用这个库作为有限场景的替代。

项目Github：<https://github.com/DapengSusu/jformat>



#### 扩展 —— 轻量级日志记录工具

基于 `jumper::format(...)` 的衍生产物：简易日志工具 `LogTracer`，提供一系列静态方法控制日志输出，支持 `Debug`、`Info`、`Warning`、`Error` 四种基本日志级别，可随时改变日志级别，在将log输出到终端的同时，也会将log写入指定的文件（是否需要写入文件可自行控制）。

请看以下示例：

```c++
// 设置当前环境log输出级别，并指定log文件保存路径。如果不需要log文件，可省略
// LogTracer::InitialTracer(jumper::LV_DEBUG, "./logtracer.txt");

// 也可以单独设置log输出级别，只有log不低于设置的级别才能输出显示
// LV_DEBUG = 1,
// LV_INFO = 2,
// LV_WARNING = 3,
// LV_ERROR = 4,
LogTracer::SetLogLevel(jumper::LV_DEBUG);

// 注意，只有重载了 << 运算符的对象才能直接作为输出参数
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

// 格式语句中，必须是成对匹配的"{}"格式符，成功匹配的格式符中间的内容将被忽略
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

// 随时改变log输出级别，这将会影响之后的log输出，现在级别低于Info的log（Debug级别的log）将不再输出！
LogTracer::SetLogLevel(jumper::LV_INFO);

// log级别低于当前设置的log级别，将不再输出显示（也不会被写入文件记录！）
LogTracer::LoglnDebug("You can NOT see me! hahaha");

// 立即关闭log文件，之后的log将不会写入文件，但仍然会在终端输出。
// 可选，默认在程序结束时自动关闭
// LogTracer::FinalTracer();

// 注意，不能多个线程向同一个文件写入，可能会导致文件内容格式混乱！
```

终端输出：

![image-20221128144848686](./assets/logtracer.png)

> __对于 `LogTracer`，我们依旧可以使用前面提到的技巧来减少重复创建相同 `Fmt` 的开销。__

[format]: https://zh.cppreference.com/w/cpp/header/format	"c++20 format"
