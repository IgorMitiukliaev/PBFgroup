#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fstream>

using namespace std;

class Server {
private:
    int port;
    int server_fd;
    struct sockaddr_in address;
    pthread_t thread_id;

public:
    Server(int port) {
        this->port = port;
        this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->server_fd == -1) {
            perror("Error: Failed to create socket");
            exit(EXIT_FAILURE);
        }

        memset(&this->address, 0, sizeof(this->address));
        this->address.sin_family = AF_INET;
        this->address.sin_addr.s_addr = INADDR_ANY;
        this->address.sin_port = htons(this->port);

        if (bind(this->server_fd, (struct sockaddr *) &this->address, sizeof(this->address)) == -1) {
            perror("Error: Failed to bind the socket to the address");
            exit(EXIT_FAILURE);
        }

        if (listen(this->server_fd, 5) == -1) {
            perror("Error: Failed to listen on the socket");
            exit(EXIT_FAILURE);
        }

        cout << "Server is listening on port " << this->port << endl;
    }

    void start() {
        while (true) {
            int client_fd;
            struct sockaddr_in client_address;
            socklen_t client_address_len = sizeof(client_address);

            client_fd = accept(this->server_fd, (struct sockaddr *) &client_address, &client_address_len);
            if (client_fd == -1) {
                perror("Error: Failed to accept connection from the client");
                continue;
            }

            cout << "New client connected: " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << endl;

            pthread_create(&this->thread_id, NULL, &Server::handle_client, (void *) &client_fd);
        }
    }

    static void *handle_client(void *arg) {
        int client_fd = *((int *) arg);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

        if (bytes_received == -1) {
            perror("Error: Failed to receive data from the client");
            close(client_fd);
            pthread_exit(NULL);
        }

        if (bytes_received == 0) {
            cout << "Client disconnected" << endl;
            close(client_fd);
            pthread_exit(NULL);
        }

        string message(buffer);
        cout << "Received message: " << message << endl;

        ofstream log_file;
        log_file.open("log.txt", ios::app);
        if (!log_file.is_open()) {
            perror("Error: Failed to open log file");
            close(client_fd);
            pthread_exit(NULL);
        }

        log_file << message << endl;
        log_file.close();

        close(client_fd);
        pthread_exit(NULL);
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./server <port>" << endl;
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    Server server(port);
    server.start();

    return 0;
}
