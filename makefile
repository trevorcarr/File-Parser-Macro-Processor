#	Luke Jasem, Tyler Grass, Trevor Carr
#	cssc0614
#	Team Indiana
#	Makefile
#	CS530, Fall 2016


TORM = macroPP macroPP.o file_parser.o

CC = g++
CCFLAGS = -g -O3 -Wall -Wno-deprecated -Wcast-qual -Wwrite-strings

macroPP:	macroPP.o file_parser.o
	${CC} ${CCFLAGS} -o macroPP macroPP.o file_parser.o
    
macroPP.o:	macroPP.cpp 
	${CC} ${CCFLAGS} -c macroPP.cpp
    
file_parser.o:	file_parser.cc file_parser.h
	${CC} ${CCFLAGS} -c file_parser.cc  

clean:
	'rm' -f ${TORM}