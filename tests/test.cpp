// Copyright 2021 Andrey Vedeneev <vedvedved2003@gmail.com>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <NetClient.hpp>

class MockClient : public net::client_interface {
public:
    MOCK_METHOD(void, Send, (const std::string& msg));
    MOCK_METHOD(bool, Connect, (const std::string& host, const uint16_t port));
};

TEST(Stack_1_Tests, ConstructorsTest) {
    MockClient mc;

    EXPECT_CALL(mc, Connect("127.0.0.1", 6969))
            .Times(1);

    mc.Connect("127.0.0.1", 6969);

    EXPECT_CALL(mc, Send("test msg"))
            .Times(1);

    mc.Send("test msg");
}
