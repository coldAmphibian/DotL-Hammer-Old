
CLANG_PP=clang++

CLANG_PP_COMPILE_OPTS=-std=c++98
CLANG_PP_LINK_OPTS=


build: mapConverter

clean:
	rm -f mapConverter
	rm -fr objectFiles

mapConverter: objectFiles/main.o
	${CLANG_PP} ${CLANG_PP_LINK_OPTS} -o mapConverter objectFiles/main.o

objectFiles/main.o: main.cpp
	mkdir -p objectFiles
	${CLANG_PP} ${CLANG_PP_COMPILE_OPTS} -c -o objectFiles/main.o main.cpp
