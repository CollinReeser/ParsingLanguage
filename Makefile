all:	parserTest

parserTest:		parserTest.cpp ParseLang.cpp Statement.cpp ./lexer/lexigraph.o
	g++ -g parserTest.cpp ParseLang.cpp Statement.cpp -o parserTest \
		./lexer/lexigraph.o

./lexer/lexigraph.o:
	cd lexer && $(MAKE)

clean:
	rm parserTest