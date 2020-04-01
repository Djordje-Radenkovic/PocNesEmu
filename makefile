
all:
	mkdir -p bin/Linux/x64/
	g++ src/*.cpp lib/fmtlib/src/*.cc -o bin/Linux/x64/PocNesEmu -Ilib/fmtlib/include
	chmod u+x bin/Linux/x64/PocNesEmu

