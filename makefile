CC = gcc
PATH1 = concentrador

all: compile run

compile: 
	mkdir -p out
	$(CC) concentrador/interface.c concentrador/communication.c concentrador/utils.c concentrador/rs232/rs232.c -lm -o out/interface.o
	$(CC) concentrador/server.c -o out/server.o
	$(CC) sensors/udp_client.c -o out/udp_client.o
config: 
	sh scripts/connection2.sh
	sleep 2
	sudo rfcomm connect /dev/rfcomm0 8C:AA:B5:B5:67:E2 1 &
run:
	./out/interface.o