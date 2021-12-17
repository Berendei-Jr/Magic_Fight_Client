#ifndef CLIENT_APP_NETCLIENT_HPP
#define CLIENT_APP_NETCLIENT_HPP

#include <NetConnection.hpp>

namespace net
{
    class client_interface
    {
    public:

        tsqueue<std::string> _in_logic_messages;

        explicit client_interface(bool encr): _socket(_context), _encryption(encr), _ptr_xtea(std::make_shared<xtea3>()) {}
        bool Ready()
        {
            return !_in_logic_messages.empty();
        }

        std::string Get()
        {
            return _in_logic_messages.pop_front();
        }

        void Send(const std::string& msg)
        {
            if (IsConnected())
            {
                message m;
                m.header.id = MsgTypes::Logic;

                if (_encryption)
                {
                    uint8_t* tmp_ptr = _ptr_xtea->data_crypt((uint8_t *) msg.c_str(), key, msg.length() + 1);
                    if (tmp_ptr == nullptr) {
                        std::cerr << "Error encrypt message\n";
                    }
                    m.header.size = _ptr_xtea->get_crypt_size();
                    m.body;
                   for (size_t i = 0; i < m.header.size; i++)
                    {
                        m.body.push_back(tmp_ptr[i]);
                    }
                } else {
                    m.header.size = msg.size();
                    m.body;
                    for (auto& it : msg)
                    {
                        m.body.push_back(it);
                    }
                }
                _connection->Send(m);
            }
        }

        bool Connect(const std::string& host, const uint16_t port)
        {
            try
            {
                boost::asio::ip::tcp::resolver resolver(_context);
                boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

                _connection = std::make_unique<connection>(
                        connection::owner::client, _context,
                        boost::asio::ip::tcp::socket(_context), _in_queue,
                        _encryption, _ptr_xtea);

                _connection->ConnectToServer(endpoints);
                _thrContext = std::thread([this]() { _context.run(); });
                std::thread UpdateThread([&]() {
                    while (true)
                    {
                        Update(-1, true);
                    }
                });
                UpdateThread.detach();
            }
            catch (std::exception& exc)
            {
                std::cerr << "Unable to connect to the server: " << exc.what() << std::endl;
                return false;
            }
            return true;
        }

        const std::string& Top()
        {
            return _in_logic_messages.front();
        }

        int NumOfMsgAvailable()
        {
            return Incoming().size();
        }

        ~client_interface() { Disconnect(); }

    private:

        void Disconnect()
        {
            if (IsConnected())
            {
                _connection->Disconnect();
            }
            _context.stop();
            if (_thrContext.joinable())
                _thrContext.join();
            _connection.release();
        }

        bool IsConnected()
        {
            if (_connection)
                return _connection->IsConnected();
            else
                return false;
        }

        tsqueue<std::string>& Incoming()
        {
            return _in_logic_messages;
        }

        void OnMessage(message& msg)
        {
            switch (msg.header.id)
            {
                case MsgTypes::Handshake:
                    std::cout << "Handshake received!\n";
                case MsgTypes::Logic:
                    if (!_encryption)
                    {
                        std::string tmp;
                        for (auto& it : msg.body)
                        {
                            tmp.push_back(it);
                        }
                        _in_logic_messages.push_back(tmp);
                    } else {
                        std::string tmp = std::string((char*)msg.body.data());
                        _in_logic_messages.push_back(tmp);
                    }
            }
        }

        void Update(size_t MaxMessages = -1, bool Wait = false)
        {
            if (Wait) _in_queue.wait();
            else  std::this_thread::sleep_for(std::chrono::seconds(1));

            size_t MessagesCount = 0;
            while (MessagesCount < MaxMessages && !_in_queue.empty())
            {
                auto msg = _in_queue.pop_front();
                OnMessage(msg._msg);
                MessagesCount++;
            }
        }

        bool _encryption;
        std::shared_ptr<xtea3> _ptr_xtea;
        uint32_t key[8] = {0x12, 0x55, 0xAB, 0xF8, 0x12, 0x45, 0x77, 0x1A};
        boost::asio::ip::tcp::endpoint _endpoint;
        tsqueue<owned_message> _in_queue;
        boost::asio::io_context _context;
        std::thread _thrContext;
        boost::asio::ip::tcp::socket _socket; // The one connected to the server
        std::unique_ptr<connection> _connection;
    };
}

#endif //CLIENT_APP_NETCLIENT_HPP
