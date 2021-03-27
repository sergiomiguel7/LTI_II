all:
	gcc concentrador.c utils.c rs232/rs232.c -o concentrador
run:
	sh connection.sh
	sudo rfcomm connect /dev/rfcomm0 8C:AA:B5:B5:67:E2 1 &
	./concentrador	