CXX=g++
BIN=parser
INC=headers

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.cpp)

all: flex fin

fin: lex.yy.cpp tokens.cpp tipo.cpp simbolo.cpp semantico.cpp codigointermedio.cpp parser.cpp main.cpp
	$(CXX) -I$(INC) -g -o $(BIN) $^ 

%.o: %.cpp
	$(CXX) $@ -c $<

.PHONY: clean
clean :
	rm -f $(BIN) *.o lex.yy.cpp

.PHONY: flex
flex:
	flex -o lex.yy.cpp lexer.lex
