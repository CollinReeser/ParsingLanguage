all: lexigraph.o lexerTest

lexerTest: lexigraph.o
	g++ -o lexerTest lexerTest.cpp lexigraph.o

lexigraph.o: lexer.o SourceRead.o MiscTextUtil.o StripComments.o ShuntingYard.o
	ld -r lexer.o SourceRead.o MiscTextUtil.o StripComments.o ShuntingYard.o -o\
		lexigraph.o

lexer.o:	lexer.cpp lexer.h
	g++ -c lexer.cpp
	
SourceRead.o:	SourceRead.cpp SimpleTextUtil.h
	g++ -c SourceRead.cpp
	
MiscTextUtil.o:	MiscTextUtil.cpp SimpleTextUtil.h
	g++ -c MiscTextUtil.cpp
		
StripComments.o:	StripComments.cpp SimpleTextUtil.h
	g++ -c StripComments.cpp
	
ShuntingYard.o:	ShuntingYard.cpp SimpleTextUtil.h
	g++ -c ShuntingYard.cpp

clean:
	rm lexerTest lexigraph.o lexer.o SourceRead.o MiscTextUtil.o\
		StripComments.o ShuntingYard.o