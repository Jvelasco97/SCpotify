CFLAGS = -Wall -lcurl -ansi -std=c11 -Werror -O3 -pipe -fstack-protector-strong -Wpedantic

all: spotifyC

spotifyC: *.o
	gcc $(CFLAGS) *.o -o spotifyC

%.o: %.c
	gcc $(CFLAGS) -c $^

clean:
	rm *.o spotifyC
