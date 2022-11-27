#include "gtest/gtest.h"
#include "fmt.h"

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

TEST(FmtTest, Normal)
{
    {
        Fmt fmt("{} + {  } = {}. ");

        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "{} + {  } = {}. ");
        ASSERT_EQ(fmt.to_substrings().size(), 4)
            << "expected size 4, but get " << fmt.to_substrings().size();
        std::deque<std::string> subs { "", " + ", " = ", ". " };
        EXPECT_EQ(fmt.to_substrings(), subs);
    }

    {
        Fmt fmt(std::string("this is {{ {} sdsd"));

        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "this is { {} sdsd");
        ASSERT_EQ(fmt.to_substrings().size(), 2)
            << "expected size 2, but get " << fmt.to_substrings().size();
        std::deque<std::string> subs { "this is { ", " sdsd" };
        EXPECT_EQ(fmt.to_substrings(), subs);
    }

    {
        Fmt fmt(std::string("ok{}} isd}"));

        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "ok{} isd}");
        ASSERT_EQ(fmt.to_substrings().size(), 2)
            << "expected size 2, but get " << fmt.to_substrings().size();
        std::deque<std::string> subs { "ok", "" };
        EXPECT_EQ(fmt.to_substrings(), subs);
    }

    {
        Fmt fmt(std::string("a = {}"));

        ASSERT_TRUE(fmt.is_ok());

        // Fmt fmtCopy(fmt); // no!
        Fmt fmtMove(std::move(fmt)); // ok!

        ASSERT_TRUE(fmtMove.is_ok());
        ASSERT_FALSE(fmt.is_ok()); // No longer valid after moving
    }
}

TEST(FmtTest, Abnormal)
{
    {
        Fmt fmt("so }{{}");

        ASSERT_FALSE(fmt.is_ok());
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Running main() from << " << __FILE__ << "\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();   
}
