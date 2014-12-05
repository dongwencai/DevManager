CC ?=gcc
CFLAG :=-g -Wall
SRC :=$(wildcard *.c)
OBJ :=$(SRC:%.c=%.o)
main:$(OBJ)
	$(CC) -o $@ $^ -lpthread -ldl
$(OBJ):%.o:%.c
	$(CC) -c $^ -o $@
clean:
	rm -rf *.o main
