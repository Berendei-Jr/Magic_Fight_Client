#include <NetClient.hpp>
#include <SFML/Graphics.hpp>

void Clear()
{
    std::cout << "\033[H\033[2J\033[3J";
}

void Pause()
{
    std::cout << "\nPress Enter to continue...\n";
    std::cin.ignore();
}

int main(int argc, char* argv[])
{

    std::string host = "127.0.0.1";
    bool encryption = true;

    if (argc == 2 && std::string(argv[1]) == "test")
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

    while (true)
    {
        Clear();
        std::cout << "\t~~~~~ Welcome to the Magic Fight! ~~~~~\n Choose your option:\n"
                  << "1) Sign up\n2) Sign in\n3) Exit\n";
        char Ch;
        std::cin >> Ch;
        if (Ch == '3')
            exit(EXIT_SUCCESS);
        else if (Ch == '1')
        {
            Clear();
            std::cout << "\tEnter nickname: ";
            std::string name, pass;
            std::cin.ignore();
            getline(std::cin, name);
            std::cout << "\tEnter password: ";
            getline(std::cin, pass);
            if (Client.Register(name, pass)) {
                Clear();
                std::cout << "You successfully signed up!\n";
                Pause();
                continue;
            } else {
                std::cerr << "Invalid info!\n";
                Pause();
                continue;
            }
        } else if (Ch == '2')
        {
            Clear();
            std::cout << "\tEnter nickname: ";
            std::string name, pass;
            std::cin.ignore();
            getline(std::cin, name);
            std::cout << "\tEnter password: ";
            getline(std::cin, pass);
            if (Client.Login(name, pass)) {
                Clear();
                std::cout << "You successfully signed in!\n";
                Pause();
                break;
            } else {
                std::cerr << "Invalid info!\n";
                Pause();
                continue;
            }
        }
    }
    //std::cin.get();
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
