CC ?=gcc
CFLAG :=-g -Wall
SRC :=$(wildcard *.c)
OBJ :=$(SRC:%.c=%.o)
deviceManager:$(OBJ)
	$(CC) $(CFLAG) -o $@ $^ -lpthread -ldl
$(OBJ):%.o:%.c
	$(CC) $(CFLAG) -c $^ -o $@
dev:
	cd device && make
devclean:
	cd device && make clean
demo:
	cd demo && make
democlean:
	cd demo && make clean
clean:
	rm -rf *.o deviceManager
