SRC = src/calc.c \
	src/calc.h \
	src/calcWeight.c \
	src/calcWeight.h \
	src/dataentry.h \
	src/digitalMap.c \
	src/digitalMap.h \
	src/draw.c \
	src/draw.h \
	src/geom.c \
	src/geom.h \
	src/index.c \
	src/index.h \
	src/jogl.h \
	src/joglIntel.c \
	src/jstring.c \
	src/jstring.h \
	src/main.c \
	src/main.h \
	src/mapData.c \
	src/mapData.h \
	src/mapitem.c \
	src/mapitem.h \
	src/move.c \
	src/move.h \
	src/readdata.c \
	src/rgb.c \
	src/rgb.h \
	src/showGuide.c \
	src/showGuide.h \
	src/sub.c \
	src/sub.h \
	src/toc.c \
	src/toc.h \
	src/winpos.h

wing: ${SRC}
	cc -v src/*.c -framework OpenGL -framework GLUT -o wing
push:
	git push pitecan.com:/home/masui/git/WING.git
	git push git@github.com:masui/WING.git
