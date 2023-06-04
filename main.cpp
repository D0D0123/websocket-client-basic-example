#include <asio.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>


#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client;

class basic_ws_client {
public:
    basic_ws_client(std::string uri): m_uri(uri) {
        /* These disable logs from the websocketpp library. 
           Comment these two lines out if you wish to see them, although it will make the terminal output cluttered. 
        */
        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        /* Initialize the transport system underlying the endpoint and set it to perpetual mode. 
           In perpetual mode the endpoint's processing loop will not exit automatically when it has no connections / disconnects.
           This is necessary if we are trying to implement reconnects
        */ 
        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

        /* Starts a thread that will run the endpoint's processing loop. 
           This will allow the endpoint to process incoming messages and dispatch them to the appropriate handlers.
           This needs to run in a separate thread because it is a blocking call.
        */
        m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);
    }

    ~basic_ws_client() {
        // the destructor will stop the endpoint's processing loop and join the thread
        m_endpoint.stop_perpetual();
        m_thread->join();
    }

    websocketpp::lib::error_code send(std::string msg) {
        websocketpp::lib::error_code err;
        m_endpoint.send(m_connection_hdl, msg, websocketpp::frame::opcode::text, err);
        return err;
    }

    /**
     * @brief Try to connect to the server. If successful, the new connection handle will be stored in m_connection_hdl.
     */
    void try_connect() {
        websocketpp::lib::error_code err_code;
        client::connection_ptr connection = m_endpoint.get_connection(m_uri, err_code);
        if (err_code) {
            std::cerr << "Error connecting: " << err_code.message() << std::endl;
            return;
        }

        // websocketpp uses handle objects to track connections. Connection pointers should not be directly used after calling connect.
        // See https://github.com/zaphoyd/websocketpp/blob/master/tutorials/utility_client/utility_client.md#terminology-connection_hdl
        m_connection_hdl = connection->get_handle();
        m_endpoint.connect(connection);
    }

private:
    /**
     * The websocketpp endpoint object.
     */
    client m_endpoint;
    /**
     * The current connection handle.
     */
    websocketpp::connection_hdl m_connection_hdl;
    /**
     * The thread that will run the endpoint's processing loop.
     */
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    /**
     * The URI of the websocket server.
     */
    std::string m_uri;

};


int main() {
    basic_ws_client ws = basic_ws_client("ws://127.0.0.1:12000");
    ws.try_connect();

    std::string msg;
    while (true) {
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