CC :=gcc -g -Wall
SRC :=$(wildcard *.c)
OBJ :=$(SRC:%.c=%.o)
deviceManager:$(OBJ)
	$(CC) -o $@ $^ -lpthread -ldl
$(OBJ):%.o:%.c
	$(CC) -c $^ -o $@
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
