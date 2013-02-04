UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(UNAME), Linux) #So I use the Intel c compiler on os-class
CC = icc
CXX = icpc
CFLAGS = -Wall -std=c99 -openmp -O3 -g -I.
CXXFLAGS = -Wall -openmp -O3 -g
#overkill on the flags, but that doesn't hurt anything
LDFLAGS = -lrt -lpthread 
#replace this if you want to change the output name
endif

THREADEDTARGET = threaded_primes
MULTIPROCESSTARGET = process_primes

#any headers go here
INCLUDES = 

#any .c or .cpp files go here
THREADEDSOURCE = threaded_primes.c
MULTIPROCSOURCE = process_primes.c

#My Latex file.
TARGET = CS311_assignment5

#default is to compile
default: thread process pdf

#depends on all of you source and header files
thread: ${THREADEDSOURCE} ${INCLUDES}
		${CC} ${CFLAGS} ${THREADEDSOURCE} -o ${THREADEDTARGET} ${LDFLAGS}

process: ${MULTIPROCSOURCE} ${INCLUDES}
		${CC} ${CFLAGS} ${MULTIPROCSOURCE} -o ${MULTIPROCESSTARGET} ${LDFLAGS}

debug: ${SOURCE} ${INCLUDES}
		${CC} ${CFLAGS} ${SOURCE} -o ${TARGET} ${LDFLAGS} -DDEBUG

clean:
	rm -rf ${MULTIPROCESSTARGET} ${THREADEDTARGET} *.dSYM ${TARGET}.dvi ${TARGET}.aux ${TARGET}.ps ${TARGET}.pdf ${TARGET}.out ${TARGET}.log

dvi: ${TARGET}.tex 
#	pygmentize the input source file -- THIS NAME SHOULD BE SAFE
#	pygmentize -f latex -o __${SRC}.tex ${SRC}
#	run latex twice to get references correct
	latex ${TARGET}.tex
#	you can also have a bibtex line here
#	bibtex $(TARGET).tex
	#latex $(TARGET).tex
#	remove the pygmentized output to avoid cluttering up the directory
	#rm __${SRC}.tex

ps: dvi
	dvips -R -Poutline -t letter ${TARGET}.dvi -o ${TARGET}.ps

pdf: ps
	ps2pdf ${TARGET}.ps

