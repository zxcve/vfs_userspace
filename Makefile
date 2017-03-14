INSTALL_TARGET=user@192.168.53.89:~

all:
	gcc -pthread test.c -o test
clean:
	rm test
install:all
	scp test test.sh $(INSTALL_TARGET)
