all: bin/server bin/client

bin/server: src/server.c
		gcc src/server.c -o bin/server -lpthread

server:
		./bin/server

bin/client: src/client.c
		gcc src/client.c -o bin/client

client:
		./bin/client

clean:
		rm -rf bin/*
