CC :=gcc -g -Wall
SRC :=$(wildcard *.c)
OBJ :=$(SRC:%.c=%.so)
main:$(OBJ)
	echo "compiling device so"
$(OBJ):%.so:%.c
	$(CC) $^ -fPIC -shared -o $@

clean:
	rm -rf *.so
