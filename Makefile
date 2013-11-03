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
wingapp: wing
	-/bin/rm -r -f WING.app
	mkdir WING.app
	mkdir WING.app/Contents
	echo '<?xml version="1.0" encoding="UTF-8"?>' > WING.app/Contents/Info.plist
	echo '<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> WING.app/Contents/Info.plist
	echo '<plist version="1.0">' >> WING.app/Contents/Info.plist
	echo '  <dict>' >> WING.app/Contents/Info.plist
	echo '    <key>CFBundleExecutable</key>' >> WING.app/Contents/Info.plist
	echo '    <string>wing</string>' >> WING.app/Contents/Info.plist
	echo '    <key>CFBundleIconFile</key>' >> WING.app/Contents/Info.plist
	echo '    <string>icon.icns</string>' >> WING.app/Contents/Info.plist
	echo '    <key>CFBundlePackageType</key>' >> WING.app/Contents/Info.plist
	echo '    <string>APPL</string>' >> WING.app/Contents/Info.plist
	echo '    <key>CFBundleSignature</key>' >> WING.app/Contents/Info.plist
	echo '    <string>????</string>' >> WING.app/Contents/Info.plist
	echo '  </dict>' >> WING.app/Contents/Info.plist
	echo '</plist>' >> WING.app/Contents/Info.plist
	mkdir WING.app/Contents/MacOS
	cp wing WING.app/Contents/MacOS
	mkdir WING.app/Contents/Resources
	cp -r data WING.app/Contents/Resources
	cp -r fonts WING.app/Contents/Resources
	convert res/wing.png res/wing.tiff
	tiff2icns res/wing.tiff WING.app/Contents/Resources/icon.icns
wingdmg: wingapp
	/bin/rm -f WING.dmg
	hdiutil create -srcfolder WING.app -volname WING WING.dmg


push:
	git push pitecan.com:/home/masui/git/WING.git
	git push git@github.com:masui/WING.git

clean:
	-/bin/rm -f src/*.o
	-/bin/rm -f wing
	-/bin/rm -r -f WING.app
	-/bin/rm -f WING.dmg


