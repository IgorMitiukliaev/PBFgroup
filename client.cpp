#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

class Client {
 private:
  string client_name;
  int port;
  int client_fd;
  struct sockaddr_in server_address;

 public:
  Client(string client_name, int port) {
    this->client_name = client_name;
    this->port = port;

    if (inet_pton(AF_INET, "127.0.0.1", &this->server_address.sin_addr) == -1) {
      perror("Error: Failed to convert IP address");
      exit(EXIT_FAILURE);
    }
  }

  void send_message() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
      perror("Error: Failed to create a socket");
      exit(EXIT_FAILURE);
    }

    memset(&this->server_address, 0, sizeof(this->server_address));
    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(this->port);

    if (connect(client_fd, (struct sockaddr *)&this->server_address,
                sizeof(server_address)) == -1) {
      perror("Error: Failed to connect to the server");
      exit(EXIT_FAILURE);
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

auto now = chrono::system_clock::now();
auto time = chrono::system_clock::to_time_t(now);
auto milliseconds = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

struct tm* timeinfo = localtime(&time);
char time_str[20];
strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

snprintf(buffer, sizeof(buffer), "[%s.%03d] \"%s\"", time_str, (int)milliseconds.count(), this->client_name.c_str());

    ssize_t bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
      perror("Error: Failed to send data to the server");
      exit(EXIT_FAILURE);
    }
    close(client_fd); 
  }
};

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cerr << "Usage: ./client <client_name> <server_port> <connection_period, s>"
         << endl;
    exit(EXIT_FAILURE);
  }

  string client_name = argv[1];
  int server_port = atoi(argv[2]);
  int connection_period = atoi(argv[3]);

  Client client(client_name, server_port);

  while (true) {
    client.send_message();
    sleep(connection_period);
  }

  return 0;
}
