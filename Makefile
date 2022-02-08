CFLAGS:=-Wall -Wextra -Werror -I include

hw_01: obj/main.o obj/bmp.o obj/stego.o
	gcc obj/bmp.o obj/main.o obj/stego.o -o hw_01

obj:
	mkdir obj

obj/main.o: include/bmp.h src/main.c | obj
	gcc -c $(CFLAGS) src/main.c -o obj/main.o

obj/bmp.o: include/bmp.h src/bmp.c | obj
	gcc -c $(CFLAGS) src/bmp.c -o obj/bmp.o
	
obj/stego.o: include/stego.h src/stego.c | obj
	gcc -c $(CFLAGS) src/stego.c -o obj/stego.o

.PHONY: clean
clean:
	rm -rf hw_01 obj

