CC = gcc
CFLAGS = -Wall -Wextra -g
OBJ = main.o pid_scanner.o readfiles.o
EXEC = friendlytop

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lncurses

main.o: main.c pid_scanner.h readfiles.h
	$(CC) $(CFLAGS) -c main.c

pid_scanner.o: pid_scanner.c pid_scanner.h
	$(CC) $(CFLAGS) -c pid_scanner.c

readfiles.o: readfiles.c readfiles.h
	$(CC) $(CFLAGS) -c readfiles.c

clean:
	rm -f *.o $(EXEC)
