CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=
TARGET=db

db: main.o
	$(CC) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(TARGET)
