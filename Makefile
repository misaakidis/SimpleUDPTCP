CC=gcc -w -s
CFLAGS=-std=c99 -w
HEADERS=$(wildcard *.h)

%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

all : udpserver udpclient tcpclient tcpserver

udpserver : stringUtils.o udpserver.o
	@$(CC) $(CFLAGS) stringUtils.o udpserver.o -o $@

udpclient : udpclient.o
	@$(CC) $(CFLAGS) udpclient.o -o $@

tcpclient : tcpclient.o
	@$(CC) $(CFLAGS) tcpclient.o -o $@

tcpserver : tcpserver.o
	@$(CC) $(CFLAGS) tcpserver.o -o $@


	@echo -e "\e[01;34mBuild was successful.\e[00m"
	@make clean


clean:
	@rm -f *.o
	@echo -e "\e[01;33mObject files were removed.\e[00m"

start:
	@./udpserver login 2400 &
	@./tcpserver 2424 &
