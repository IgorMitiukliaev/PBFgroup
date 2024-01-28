CC	= g++ -std=c++17
CXXFLAGS 	= -Wall -Wextra

server:
	$(CC) $(CXXFLAGS) server.cpp -o server.out

client:
	$(CC) $(CXXFLAGS) client.cpp -o client.out

clean:
	rm -rf *.out
