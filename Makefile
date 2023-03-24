CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=
TARGET=db

db: main.o input.o command.o row.o table.o
	$(CC) $^ -o $@ $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@
input.o: input.c
	$(CC) $(CFLAGS) -c $< -o $@
command.o: command.c
	$(CC) $(CFLAGS) -c $< -o $@
row.o: row.c
	$(CC) $(CFLAGS) -c $< -o $@
table.o: table.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(TARGET)
