CFLAGS += -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE
CC = g++

all: dif


dif: main.o tree.o treedump.o dif.o treeread.o
	$(CC) -o dif.exe obj/main.o obj/tree.o obj/treedump.o obj/dif.o obj/treeread.o $(CFLAGS)

main.o: main.cpp
	$(CC) -o obj/main.o main.cpp -c $(CFLAGS)

tree.o: tree/tree.cpp 
	$(CC) -o obj/tree.o tree/tree.cpp -c $(CFLAGS)

treedump.o: tree/treedump.cpp 
	$(CC) -o obj/treedump.o tree/treedump.cpp -c $(CFLAGS)

dif.o: dif.cpp 
	$(CC) -o obj/dif.o dif.cpp -c $(CFLAGS)

treeread.o: treeread.cpp 
	$(CC) -o obj/treeread.o treeread.cpp -c $(CFLAGS)

clean:
	rm *.o
	clear
	
.PHONY: clean