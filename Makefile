CFLAGS = -Wall -lcurl -g

all: spotifyC

spotifyC: *.o
	gcc $(CFLAGS) *.o -o spotifyC

%.o: %.c
	gcc $(CFLAGS) -c $^

clean:
	rm *.o spotifyC
