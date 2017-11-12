CC=clang
CXX=clang++

ARNOLD_PATH=/Users/zeno/Arnold-5.0.2.0-darwin

CXXFLAGS=-std=c++11 -Wall -O3 -shared -fPIC -I${ARNOLD_PATH}/include
LDFLAGS=-L${ARNOLD_PATH}/bin -lai

HEADERS=\

.PHONY=all clean

all: dust

dust: Makefile src/dust.cpp ${HEADERS}
	${CXX} ${CXXFLAGS} src/dust.cpp -o bin/dusty.dylib ${LDFLAGS}

clean:
	rm -f dusty
