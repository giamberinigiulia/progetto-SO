all: RBC avvio

avvio: registro.o logicaTreni.o avvio.o
	cc registro.o logicaTreni.o avvio.o -o avvio

avvio.o: src/avvio.c src/registro.h src/logicaTreni.h
	cc -c src/avvio.c

logicaTreni.o: src/logicaTreni.c src/logicaTreni.h
	cc -c src/logicaTreni.c

registro.o: src/registro.c src/registro.h
	cc -c src/registro.c

RBC: src/RBC.c
	cc src/RBC.c -o RBC

install:
	mkdir bin
	mv *.o bin
	mv avvio bin
	mv RBC bin

clean:
	rm -r bin
	rm -r directoryMA
	rm -r log