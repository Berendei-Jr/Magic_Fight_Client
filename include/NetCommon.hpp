#ifndef CLIENT_APP_NETCOMMON_HPP
#define CLIENT_APP_NETCOMMON_HPP

#include <cstring>
#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <chrono>
#include <boost/asio.hpp>

namespace net
{
    enum class MsgTypes : uint32_t
    {
        Handshake,
        Logic
    };

    struct mes_header
    {
        MsgTypes id{};
        uint32_t size = 0;
    };

    struct message
    {
        mes_header header{};
        std::vector<uint8_t> body;

        size_t size() const
        {
            return sizeof(header) + header.size;
        }

        friend std::ostream& operator << (std::ostream& out, const message& msg)
        {
            out << "ID: " << int(msg.header.id) << " Size: " << msg.header.size << " Body: ";
            for (auto& it: msg.body)
            {
                out << it;
            }
            out << "\n";
            return out;
        }
    };

    class connection;

    struct owned_message
    {
        std::shared_ptr<connection> _remote = nullptr;
        message _msg;
        friend std::ostream& operator << (std::ostream& out, const owned_message& msg)
        {
            out << msg._msg;
            return out;
        }
    };

}

#endif //CLIENT_APP_NETCOMMON_HPP
