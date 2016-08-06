all: compile

run: compile
	./msdosdir.out input/samplefat.bin

compile: msdosdir.out msdosextr.out

msdosdir.out: bin/fat12.o bin/msdosdir.o
	gcc -omsdosdir.out bin/fat12.o bin/msdosdir.o

msdosextr.out: bin/fat12.o bin/msdosextr.o
	gcc -omsdosextr.out bin/fat12.o bin/msdosextr.o

bin/fat12.o: src/fat12.c
	gcc -Iinclude/ -obin/fat12.o -c src/fat12.c

bin/msdosdir.o: src/msdosdir.c
	gcc -Iinclude/ -obin/msdosdir.o -c src/msdosdir.c

bin/msdosextr.o: src/msdosextr.c
	gcc -Iinclude/ -obin/msdosextr.o -c src/msdosextr.c

clean:
	rm *.out bin/*
