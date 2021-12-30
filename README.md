[![Build Status](https://app.travis-ci.com/Berendei-Jr/Magic_Fight_Client.svg?token=yrbcZndhR45esCVoxGyJ&branch=main)](https://app.travis-ci.com/Berendei-Jr/Magic_Fight_Client)
# Magic_Fight_Client
Для запуска сети необходимо использовать команды:
```
net::client_interface Client;
Client.Connect("127.0.0.1", 60000);
```
Объект Client имеет следующие методы:
1) bool Ready() - имеется ли хоть одно входящее сообщение от сервера.
2) std::string Get() - изъять ссобщение, присланное сервером, из очереди (вначале надо проверять наличие такового через Ready() ).
3) void Send(const std::string&) - отправить сообщение серверу (сообщение - строка std::srting).
4) int NumOfMsgAvailable() - сколько сообщений от сервера находятся в очереди.
5) const std::string& Top() - просмотреть верхнее сообщение в очереди.
