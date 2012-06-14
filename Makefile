all:	parserTest

parserTest:		parserTest.cpp parserLang.cpp lexigraph.o
	g++ -g parserTest.cpp parserLang.cpp -o parserTest lexigraph.o
