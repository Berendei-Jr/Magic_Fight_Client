#include <NetClient.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
    /*
    std::string str_tst = "Hello, bitch!\n";
    std::unique_ptr<xtea3> ptr_xtea_lib = std::make_unique<xtea3>();
    uint32_t key[8] = {0x12, 0x55, 0xAB, 0xF8, 0x12, 0x45, 0x77, 0x1A};
    uint8_t *p_crypt_data = ptr_xtea_lib->data_crypt((uint8_t*)str_tst.c_str(), key, str_tst.length() + 1);
    if (p_crypt_data == nullptr)
    {
        std::cout << "Error decrypt fdata \n";
        return (-1);
    }

    std::cout << "CRYPT_STR: \n";
    std::cout << "CRYPT_STR SIZE = " << ptr_xtea_lib->get_crypt_size() << "\n";
    for (int i = 0; i < ptr_xtea_lib->get_crypt_size(); i++)
    {
        std::cout << p_crypt_data[i];
    }

    uint8_t *p_decrypt_data = ptr_xtea_lib->data_decrypt((uint8_t*)p_crypt_data, key, ptr_xtea_lib->get_crypt_size());
    if (p_decrypt_data == nullptr)
    {
        std::cout << "Error decrypt fdata \n";
        return (-1);
    }

    //Очистим строку
    str_tst.clear();

    //Прировняем указатель на расшифрованные данные нашей строки
    str_tst = (char*)p_decrypt_data;

    std::cout << "\n\nDECRYPT_STR: \n";

    std::cout << "SIZE DECRYPT STR: " << ptr_xtea_lib->get_decrypt_size() <<" \n";
    std::cout << "SIZE DECRYPT STRING: " << str_tst.length() << " \n";
    std::cout << str_tst.c_str() << " \n";

    */

    std::string host = "127.0.0.1";
    bool encryption = true;

    if (argc == 3)
    {
        host = argv[1];
        encryption = std::stoi(argv[2]);
    }

    net::client_interface Client(encryption);
    Client.Connect(host, 6969);

    //Client.Send("Hello test!\n");

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


            window.display();
        }
    }
}
