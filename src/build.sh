cc -c -g *.c
rm jogl.o
cc -o wing -g -framework GLUT -framework OpenGL -framework Cocoa *.o

