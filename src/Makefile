COBJ =	./main.o \
	./calc.o \
	./calcWeight.o \
	./digitalMap.o \
	./draw.o \
	./geom.o \
	./index.o \
	./jogl.o \
	./jstring.o \
	./main.o \
	./mapData.o \
	./mapitem.o \
	./move.o \
	./readdata.o \
	./rgb.o \
	./showGuide.o \
	./sub.o \
	./toc.o

INC =	./calc.h \
	./calcWeight.h \
	./dataentry.h \
	./digitalMap.h \
	./draw.h \
	./geom.h \
	./index.h \
	./jogl.h \
	./jstring.h \
	./main.h \
	./mapData.h \
	./mapitem.h \
	./move.h \
	./rgb.h \
	./showGuide.h \
	./sub.h \
	./toc.h \
	./winpos.h

all: $(COBJ)
	CC -g -o wing $(COBJ) libglut.a -lGLU -lGL -lXmu -lX11 -lm

$(COBJ): $(INC)

.C.o:
	CC -c -g $<

