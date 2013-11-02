wing: src/*.c src/*.h
	cc -v src/*.c -framework OpenGL -framework GLUT -o wing
push:
	git push pitecan.com:/home/masui/git/WING.git
	git push git@github.com:masui/WING.git
