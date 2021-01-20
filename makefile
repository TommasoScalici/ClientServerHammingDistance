clean:
	rm -f client server

client:
	gcc -pthread -g -o client client.c

server:
	gcc -pthread -g -o server server.c