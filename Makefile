CC=clang
CXX=clang++

ARNOLD_PATH=/Users/zeno/Arnold-5.0.2.0-darwin

CXXFLAGS=-std=c++11 -Wall -O3 -shared -fPIC -I${ARNOLD_PATH}/include
LDFLAGS=-L${ARNOLD_PATH}/bin -lai

HEADERS=\

.PHONY=all clean

all: dusty

dusty: Makefile src/dusty.cpp ${HEADERS}
	${CXX} ${CXXFLAGS} src/dusty.cpp -o bin/dusty.dylib ${LDFLAGS}

clean:
	rm -f dusty



# compile on windows using vs17 x64 developer cmd prompt
# cl /LD /I C:\Users\Zeno\Arnold-5.0.2.0-windows\include /EHsc /O2 dust.cpp /link /LIBPATH:C:\Users\Zeno\Arnold-5.0.2.0-windows\lib ai.lib