CFLAGS = -ansi -pedantic -Wall -g

assembler.out: assembler.o macro.o print.o firstPass.o secondPass.o commonFunctions.o genericArray.o analyser.o
	gcc $(CFLAGS) *.o -o assembler.out


assembler.o: assembler.c assembler.h genericArray.h analyser.h
	gcc $(CFLAGS) -c assembler.c

macro.o: macro.c macro.h genericArray.h commonFunctions.h
	gcc $(CFLAGS) -c macro.c

print.o: print.c assembler.h genericArray.h analyser.h commonFunctions.h
	gcc $(CFLAGS) -c print.c

secondPass.o: secondPass.c assembler.h genericArray.h analyser.h
	gcc $(CFLAGS) -c secondPass.c

firstPass.o: firstPass.c assembler.h genericArray.h analyser.h
	gcc $(CFLAGS) -c firstPass.c

commonFunctions.o: commonFunctions.c commonFunctions.h
	gcc $(CFLAGS) -c commonFunctions.c

genericArray.o: genericArray.c genericArray.h
	gcc $(CFLAGS) -c genericArray.c

analyser.o: analyser.c analyser.h
	gcc $(CFLAGS) -c analyser.c



clean:
	rm -f *.o

