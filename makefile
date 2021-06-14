CC = gcc
PATH1 = concentrador

all: compile run

compile: 
	mkdir -p out
	$(CC) concentrador/interface.c concentrador/communication.c concentrador/utils.c concentrador/rs232/rs232.c -lm -lmosquitto -pthread -o out/interface.out
	$(CC) concentrador/server.c -o out/server.out
	$(CC) sensors/udp_client.c -o out/udp_client.out
	$(CC) gestor/gestor.c -pthread -I/usr/include/mysql -lmysqlclient -o out/gestor.out
config: 
	sh concentrador/scripts/connection2.sh
	sleep 2
	sudo rfcomm connect /dev/rfcomm0 8C:AA:B5:B5:67:E2 1 &
broker:
	mosquitto -c env/mosquitto.conf -v
run:
	./out/gestor.out
	