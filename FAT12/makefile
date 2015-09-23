run: compile
	./msdosdir.out input/samplefat.bin

compile: msdosdir.out

msdosdir.out: bin/fat12.o bin/msdosdir.o
	gcc -omsdosdir.out bin/fat12.o bin/msdosdir.o

bin/fat12.o: src/fat12.c
	gcc -obin/fat12.o -c src/fat12.c

bin/msdosdir.o: src/msdosdir.c
	gcc -obin/msdosdir.o -c src/msdosdir.c

clean:
	rm *.out bin/*
