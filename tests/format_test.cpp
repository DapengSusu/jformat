#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "fmt.h"
#include "format.h"

using jumper::Fmt;

class User {
public:
    User(unsigned id, const std::string& name)
        : m_id(id), m_name(name) {}
    User(unsigned id, const char* name)
        : User(id, std::string(name)) {}

    unsigned m_id;
    std::string m_name;
};

// 注意：必须重载了<<运算符的对象才可以使用 jumper::format(...)
std::ostream& operator<<(std::ostream& oss, const User& user)
{
    return oss << "id:" << user.m_id << ",name:" << user.m_name;
}

TEST(FormatTest, Normal)
{
    {
        std::string str(jumper::format("{} + {} = {}", 1, 1, 1+1));

        EXPECT_EQ(str, "1 + 1 = 2");
    }

    {
        std::string str(jumper::format("this is a char: {}", 'c'));

        EXPECT_EQ(str, "this is a char: c");
    }

    {
        Fmt fmt("5 - {}");

        fmt += " = {}";
        std::string str(jumper::format(fmt, 2, 5-2));

        EXPECT_EQ(str, "5 - 2 = 3");
    }

    {
        // 参数数量可以比格式符多，多余参数将被忽略
        auto str(jumper::format("one is {}, two is {}.", 1, 2, 3, 4, 5));

        EXPECT_EQ(str, "one is 1, two is 2.");
    }

    {
        auto str(jumper::format("A simple message.", "important"));

        EXPECT_EQ(str, "A simple message.");
    }

    {
        // User类重载了 << 运算符，所以可以直接作为参数输出
        User user(1, "Jumper");
        std::string str(jumper::format("user:{}", user));

        EXPECT_EQ(str, "user:id:1,name:Jumper");
    }

    {
        // print
        jumper::println("Input something...");
        jumper::print("A -> {}\n", 'a');
        jumper::println(std::string("d -> {}"), 'D');

        Fmt fmt("{} is my best friend.\n");
        jumper::print(fmt, "Anna");

        ASSERT_TRUE(true);
    }

    {
        // 提高性能的技巧：对相同的格式反复输出（或者格式化）时，可以提前构造Fmt对象，重复利用
        int a = 0;
        int b = 0;
        Fmt fmt("loop index: {}, a = {}, b = {}");

        ASSERT_TRUE(fmt.is_ok()); // Fmt对象必须有效

        for (auto i = 0; i != 10; ++i)
        {
            ++a;
            b += 2;

            jumper::println(fmt, i, a, b);
        }

        EXPECT_EQ(a, 10);
        EXPECT_EQ(b, 20);
    }

    {
        // 使用转义输出大括号
        Fmt fmt("This is a big bracket: {{}}.");
        std::string str(jumper::format("This is a big bracket: {{ {} }}.", 'a'));
        
        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "This is a big bracket: {}.");
        EXPECT_EQ(str, "This is a big bracket: { a }.");
    }
}

TEST(FormatTest, Abnormal)
{
    {
        // 不允许格式符数量比参数多，这将会返回空字符串
        auto str(jumper::format("one is {}, two is {}.", 1));

        EXPECT_TRUE(str.empty());
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Running main() from << " << __FILE__ << "\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();   
}
