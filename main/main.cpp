#include <NetClient.hpp>

int main() {
    net::client_interface Client;
    Client.Connect("127.0.0.1", 60000);
    while (true)
    {
        if (Client.Ready())
        {
          //  std::cout << "size: " << Client.NumOfMsgAvailable() << std::endl;
            std::cout << "MSG GOT: " << Client.Get() << std::endl;
        }
        Client.Send("Hello bitch");
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

}
