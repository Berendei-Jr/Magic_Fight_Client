#ifndef CLIENT_APP_NETCONNECTION_HPP
#define CLIENT_APP_NETCONNECTION_HPP

#include "../../Magic_Fight_Client/include/NetCommon.hpp"
#include <NetThreadSafeQueue.hpp>
#include <xtea3_lib/xtea3.h>

namespace net
{
    class connection : public std::enable_shared_from_this<connection>
    {
    public:
        enum class owner
        {
            server,
            client
        };

        connection(owner parent, boost::asio::io_context& context,
                   boost::asio::ip::tcp::socket socket,
                   tsqueue<owned_message>& in_queue, bool encr) :
                   _context(context), _socket(std::move(socket)),
                   _in_queue(in_queue), _encryption(encr),
                   _ptr_xtea(std::make_unique<xtea3>())
        {
            _owner = parent;
        }

        ~connection() = default;

        void ConnectToClient(uint32_t uid = 0)
        {
            if (_owner == owner::server)
            {
                if (_socket.is_open())
                {
                    _id = uid;
                    ReadHeader();
                }
            }
        }

        void ConnectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints)
        {
            if (_owner == owner::client)
            {
                boost::asio::async_connect(_socket, endpoints, [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
                {
                    if (!ec)
                    {
                        ReadHeader();
                    }
                });
            }
        }

        void Disconnect()
        {
            if (IsConnected())
                boost::asio::post(_context, [this]() { _socket.close(); });
        }
        
        bool IsConnected() const
        {
            return _socket.is_open();
        }

        void Send(const message& msg)
        {
            boost::asio::post(_context, [this, msg]()
            {
                bool WritingMessage = !_out_queue.empty();
                _out_queue.push_back(msg);
                if (!WritingMessage)
                {
                    WriteHeader();
                }
            });
        }

        uint32_t GetId() const
        {
            return _id;
        }

        void ReadHeader()
        {
            boost::asio::async_read(_socket, boost::asio::buffer(&_tmp_msg.header, sizeof(mes_header)),
                                    [this](boost::system::error_code ec, size_t length)
                                    {
                                        if (!ec)
                                        {
                                            if (_tmp_msg.header.size)
                                            {
                                                _tmp_msg.body.resize(_tmp_msg.header.size);
                                                ReadBody();
                                            } else {
                                                AddToIncomingMsgQueue(true);
                                            }
                                        } else {
                                            std::cout << "[" << _id << "] Read header failed: " << ec.message() << "\n";
                                            _socket.close();
                                        }
                                    });
        }

        void ReadBody()
        {
            boost::asio::async_read(_socket, boost::asio::buffer(_tmp_msg.body.data(), _tmp_msg.body.size()),
                                    [this](boost::system::error_code ec, size_t length)
                                    {
                                        if (!ec)
                                        {
                                            AddToIncomingMsgQueue();
                                        } else {
                                            std::cout << "[" << _id << "] Read body failed: " << ec.message() << "\n";
                                            _socket.close();
                                        }
                                    });
        }

        void WriteHeader()
        {
            boost::asio::async_write(_socket, boost::asio::buffer(&_out_queue.front().header, sizeof(mes_header)),
                                     [this](boost::system::error_code ec, size_t length)
                                     {
                                         if (!ec)
                                         {
                                             if (_out_queue.front().body.size() > 0)
                                             {
                                                 WriteBody();
                                             } else {
                                                 _out_queue.pop_front();
                                                 if (!_out_queue.empty())
                                                 {
                                                     WriteHeader();
                                                 }
                                             }
                                         } else {
                                             std::cout << "[" << _id << "] Write header failed: " << ec.message() << "\n";
                                             _socket.close();
                                         }
                                     });
        }

        void WriteBody()
        {
            std::string tmp = _out_queue.front().body;
            uint8_t *p_crypt_data = _ptr_xtea->data_crypt((uint8_t*)tmp.c_str(), key, tmp.length() + 1);
            if (p_crypt_data == nullptr)
            {
                std::cerr << "Error encrypt message\n";
                WriteHeader();
            }

            boost::asio::async_write(_socket, boost::asio::buffer(p_crypt_data, _ptr_xtea->get_crypt_size()),
                                     [this](boost::system::error_code ec, size_t length)
                                     {
                                        if (!ec)
                                        {
                                            _out_queue.pop_front();
                                            if (!_out_queue.empty())
                                            {
                                                WriteHeader();
                                            }
                                        } else {
                                            std::cout << "[" << _id << "] Write body failed: " << ec.message() << "\n";
                                            _socket.close();
                                        }
                                     });
        }

        void AddToIncomingMsgQueue(bool EmptyMsg = false)
        {
            if (EmptyMsg)
            {
                ReadHeader();
                return;
            }
            if (_owner == owner::server)
                _in_queue.push_back({ this->shared_from_this(), _tmp_msg });
            else
                _in_queue.push_back({nullptr, _tmp_msg });
            ReadHeader();
        }

    private:
        boost::asio::ip::tcp::socket _socket;
        boost::asio::io_context& _context;
        tsqueue<message> _out_queue;
        tsqueue<owned_message>& _in_queue;
        owner _owner = owner::server;
        uint32_t _id = 0;
        message _tmp_msg;
        bool _encryption;
        std::unique_ptr<xtea3> _ptr_xtea;
        uint32_t key[8] = {0x12, 0x55, 0xAB, 0xF8, 0x12, 0x45, 0x77, 0x1A};
    };
}

#endif //CLIENT_APP_NETCONNECTION_HPP
