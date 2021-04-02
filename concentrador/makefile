all:
	gcc interface.c utils.c rs232/rs232.c -o out/interface.o
config:
	sh scripts/connection.sh
	sleep 2
	sudo rfcomm connect /dev/rfcomm0 8C:AA:B5:B5:67:E2 1 &
run:
	./out/interface.o	