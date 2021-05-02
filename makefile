all: server client

server: dserver.c
	gcc dserver.c -o dserver

client: dclient.c
	gcc dclient.c -o dclient

clean:
	rm dserver dclient
