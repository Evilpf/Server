#include <iostream>
#include <asio.hpp>

using namespace std;
using namespace asio;
using namespace asio::ip;

class TCPServer {
public:
    // Constructor to initialize the TCP server
    TCPServer(asio::io_context& io_context, short port)

        : acceptor_(io_context, tcp::endpoint(ip::make_address("192.168.1.77"), port)),

        socket_(io_context) {

        bool flag = false;

        do {
            // Start accepting connections
            do_accept();
            cout << "\n";
        } while (flag);
    }

private:
    // Function to asynchronously accept incoming connections
    void do_accept() {

        acceptor_.async_accept(socket_,

            [this](std::error_code ec) {

                if (!ec) {
                    // Connection accepted, print client endpoint
                    std::cout << "Connection accepted from: " << socket_.remote_endpoint() << std::endl;

                    // Prepare a welcome message
                    std::make_shared<std::string>("Welcome to the server!")->swap(data_);

                    // Start reading data from the client
                    do_read();
                }

                // Continue accepting connections
                do_accept();
            });
    }

    // Function to asynchronously read data from the client
    void do_read() {

        async_read(socket_,

            asio::buffer(data_),

            [this](std::error_code ec, std::size_t length) {
                if (!ec) {

                    // Data received, print it
                    std::cout << "Received: " << data_ << std::endl;

                    // Echo the received data back to the client
                    do_write(length);
                }
            });
    }

    // Function to asynchronously write data back to the client
    void do_write(std::size_t length) {

        async_write(socket_,
            asio::buffer(data_, length),

            [this](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {

                    // Data sent successfully, print it
                    std::cout << "Sent: " << data_ << std::endl;

                    // Continue reading data from the client
                    do_read();
                }
            });
    }

    tcp::acceptor acceptor_; // Acceptor object to accept connections
    tcp::socket socket_;     // Socket object to communicate with clients
    std::string data_;       // Buffer to hold received and to-be-sent data
};

int main() {

    try {
        asio::io_context io_context; // Create IO context

        TCPServer server(io_context, 8080); // Start TCP server on port 1234

        io_context.run(); // Run the IO context to start processing asynchronous operations

    }
    catch (std::exception& e) {

        std::cerr << e.what() << std::endl; // Handle exceptions
    }
    return 0;
}
