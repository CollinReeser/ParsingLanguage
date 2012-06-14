all:	parserTest

parserTest:		parserTest.cpp parserLang.cpp lexigraph.o
	g++ -o parserTest parserTest.cpp parserLang.cpp lexigraph.o
