### 字符串格式化 jumper::format(...)函数

#### 用法

`format`函数依赖`Fmt`对象提供基础功能，首先介绍下`Fmt`类:

> `Fmt`是一个解析指定格式生成最终的格式串，根据配对的__'{'__和__'}'__分割格式串，最终`format`根据参数依次拼接输出。
>
> `Fmt`有两种构造方式：
>
> 1. 直接指定一个格式字符串，`Fmt fmt("a = {}");`
> 2. 先构造空对象，再设置格式，`Fmt fmt;fmt.set_fmt("a = {}");`
>
> 事实上，`Fmt`对象随时都可以调用`set_fmt`修改格式。还可以使用__+__或者__+=__拼接两个`Fmt`对象，就像拼接两个字符串一样。
>
> __<span style="color:red"> 在构造`Fmt`对象之后，请务必调用`is_ok()`来检查格式是否解析成功，解析失败`Fmt`对象将持有一个空的格式。</span>__

我们可以通过创建一个`Fmt`对象来包含我们需要的格式，然后作为参数传递给`format`，更常见的情况是，我们直接使用字符串

作为格式参数：

```c++
std::string str(jumper::format("{} + {} = {}", 1, 1, 1+1));

EXPECT_EQ(str, "1 + 1 = 2");
```

当然也可以：

```c++
Fmt fmt("5 - {}");

fmt += " = {}";
std::string str(jumper::format(fmt, 2, 5-2));

EXPECT_EQ(str, "5 - 2 = 3");
```

__<span style="color:red"> 需要注意的一点是，`format`函数可以格式化的参数，必须是重载了<<运算符的对象。</span>__

另外，建议使用 `jumper::format(...)`而不是这样：

```c++
using jumper::format;

std::string str(format(...));
```

因为`C++20`标准新增了`<format>`库 [<https://zh.cppreference.com/w/cpp/header/format>]，其中也有`format`函数，但标准库的功能更加强大，为了防止潜在的命名冲突，建议明确指定作用域。

且`C++20`推广使用还需要一段时间，目前各家编译器对`<format>`支持也很差。可以使用这个库作为有限场景的替代。

Github：<https://github.com/DapengSusu/jformat>
