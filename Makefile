all:	parserTest

parserTest:		parserTest.cpp parserLang.cpp lexer/lexigraph.o
	g++ -g parserTest.cpp parserLang.cpp -o parserTest lexer/lexigraph.o
