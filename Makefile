# CFLAGS = -Wall -lcurl -fsanitize=address -g 
CFLAGS = -Wall -lcurl -g -Wall -Werror

all: spotifyC

spotifyC: *.o
	gcc $(CFLAGS) *.o -o spotifyC

%.o: %.c
	gcc $(CFLAGS) -c $^

clean:
	rm *.o spotifyC
