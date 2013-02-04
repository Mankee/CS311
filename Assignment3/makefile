CC = icc
CXX = icpc
CFLAGS = -Wall -std=c99 -openmp -O3 -g -I.
CXXFLAGS = -Wall -openmp -O3 -g

#overkill on the flags, but that doesn't hurt anything
LDFLAGS = -lrt -lpthread 

#replace this if you want to change the output name
TARGET = myar

#any headers go here
INCLUDES = 

#any .c or .cpp files go here
SOURCE = myar.c

#default is to compile
default:	compile

#depends on all of you source and header files
compile: ${SOURCE} ${INCLUDES}

#this assumes you actually are linking all of the source files together
	${CC} ${CFLAGS} ${SOURCE} -o ${TARGET} ${LDFLAGS}

debug: ${SOURCE} ${INCLUDES}
	${CC} ${CFLAGS} ${SOURCE} -o ${TARGET} ${LDFLAGS} -DDEBUG

tex = CS311_assignment3

both: pdf html

dvi: ${tex}.tex
#	pygmentize the input source file -- THIS NAME SHOULD BE SAFE
#	pygmentize -f latex -o __${SRC}.tex ${SRC}
#	run latex twice to get references correct
	latex ${tex}.tex
#	you can also have a bibtex line here
	bibtex $(tex).tex
	latex $(tex).tex
#	remove the pygmentized output to avoid cluttering up the directory
#	rm __${SRC}.tex

ps: dvi
	dvips -R -Poutline -t letter ${tex}.dvi -o ${tex}.ps

pdf: ps
	ps2pdf ${tex}.ps


html:
	cp ${tex}.tex ${HTML}.tex
	latex ${HTML}.tex
	latex2html -split 0 -noshow_section_numbers -local_icons -no_navigation -noinfo -noaddress ${HTML}

	sed 's/<BR><HR>//g' < ${HTML}/index.html > ${HTML}/index2.html
	mv ${HTML}/index2.html ${tex}.html
	rm ${HTML}.*
	rm -rf ${HTML}


