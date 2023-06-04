#include <asio.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>


#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client;

int main() {
    client c;

    c.init_asio();
    c.start_perpetual();

    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    std::string uri = "ws://127.0.0.1:12000";

    websocketpp::lib::error_code err_code;
    client::connection_ptr connection = c.get_connection(uri, err_code);

    if (err_code) {
        std::cout << "Error connecting: " << err_code.message() << std::endl;
    }

    c.connect(connection);

    // Start running the websocket
    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &c);

    int n;
    while (1) {
        connection->send((std::string)"Hi everybody", websocketpp::frame::opcode::text);
        std::cin >> n;
    }


    return 0; 
}