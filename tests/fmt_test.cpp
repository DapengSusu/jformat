#include "gtest/gtest.h"
#include "fmt.h"

using jumper::Fmt;

TEST(FmtTest, Normal)
{
    {
        Fmt fmt("{} + {  } = {}. ");

        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "{} + {  } = {}. ");
        ASSERT_EQ(fmt.subs().size(), 4)
            << "expected size 4, but get " << fmt.subs().size();
        std::deque<std::string> subs { "", " + ", " = ", ". " };
        EXPECT_EQ(fmt.subs(), subs);
    }

    {
        Fmt fmt(std::string("this is {{ {} sdsd"));

        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "this is { {} sdsd");
        ASSERT_EQ(fmt.subs().size(), 2)
            << "expected size 2, but get " << fmt.subs().size();
        std::deque<std::string> subs { "this is { ", " sdsd" };
        EXPECT_EQ(fmt.subs(), subs);
    }

    {
        Fmt fmt(std::string("ok{}} isd}"));

        ASSERT_TRUE(fmt.is_ok());
        EXPECT_EQ(fmt.to_str(), "ok{} isd}");
        ASSERT_EQ(fmt.subs().size(), 2)
            << "expected size 2, but get " << fmt.subs().size();
        std::deque<std::string> subs { "ok", "" };
        EXPECT_EQ(fmt.subs(), subs);
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
