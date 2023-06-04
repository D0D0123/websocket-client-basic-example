#include <asio.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>


#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class basic_ws_client {
public:
    basic_ws_client(std::string uri) {
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

        m_uri = uri;

        m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);
    }

    ~basic_ws_client() {
        m_endpoint.stop_perpetual();
        m_thread->join();
    }

    websocketpp::lib::error_code send(std::string msg) {
        websocketpp::lib::error_code err;
        m_endpoint.send(m_connection_hdl, msg, websocketpp::frame::opcode::text, err);
        return err;
    }

    void try_connect() {
        websocketpp::lib::error_code err_code;
        client::connection_ptr connection = m_endpoint.get_connection(m_uri, err_code);
        if (err_code) {
            std::cerr << "Error connecting: " << err_code.message() << std::endl;
        }

        m_connection_hdl = connection->get_handle();
        m_endpoint.connect(connection);
    }

private:
    client m_endpoint;
    websocketpp::connection_hdl m_connection_hdl;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    std::string m_uri;

};


int main() {
    basic_ws_client ws = basic_ws_client("ws://127.0.0.1:12000");
    ws.try_connect();

    std::string msg;
    while (1) {
        std::cout << "Write Message:" << std::endl;
        std::cin >> msg;
        websocketpp::lib::error_code err = ws.send(msg);
        if (err) {
            std::cerr << "Could not send message, disconnected from websocket server.\nAttempting to Reconnect." << std::endl;
            ws.try_connect();
        } else {
            std::cout << "Successfully sent: " << msg << std::endl;
        }
    }

    return 0; 
}