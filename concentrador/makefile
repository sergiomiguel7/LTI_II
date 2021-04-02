all:
	gcc interface2.c utils.c rs232/rs232.c -o out/interface2.o
config:
	sh connection.sh
	sudo rfcomm connect /dev/rfcomm0 8C:AA:B5:B5:67:E2 1 &
run:
	./out/interface2.o	