#include <NetClient.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{

    std::string host = "127.0.0.1";
    bool encryption = true;

    if (argc == 2 && argv[1] == "test")
    {
        exit(EXIT_SUCCESS);
    }

    if (argc == 3)
    {
        host = argv[1];
        encryption = std::stoi(argv[2]);
    }

    net::client_interface Client(encryption);
    Client.Connect(host, 6969);
    if (Client.Register("Supsun", "pass"))
    {
        std::cout << "Successfully registered!\n";
    }
    if (Client.Login("Supsun", "pass"))
    {

        std::cout << "Successfully logged!\n";
    }

    char d, act_key;
    bool b = false;

    std::map<char, int> direct = {{'a', 0},
                                  {'w', 1},
                                  {'e', 2},
                                  {'d', 3},
                                  {'x', 4},
                                  {'z', 5},
                                  {'s', 6}};
    std::map<char, int> action_map = {{'k', 0},
                                      {'o', 1},
                                      {'l', 2},
                                      {'p', 3}};
    std::map<char, int>::iterator it1, it2;
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    //! БЫДЛОКОД
    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                d = event.key.code + 97;
                if (b) {
                    it1 = direct.find(d);
                    if (it1 != direct.end())
                    {
                        Client.Send(std::to_string(it2->second)
                        + " " + std::to_string(it1->second));
                        //std::cout << it2->second <<
                        //          " " << it1->second << "\n";
                    }
                    b = false;
                } else {
                    act_key = d;
                    it2 = action_map.find(act_key);
                    b = it2 != action_map.end();
                }

            }

            window.clear(sf::Color::Black);
            if (Client.Ready())
            {
                std::cout << Client.Get() << "\n";
            }

            window.display();
        }
    }
}
