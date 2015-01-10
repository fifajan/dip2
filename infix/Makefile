all: infixlex.c infixlex.o

infixlex.c:
	bison -o infixlex.c infixlex.y

infixlex.o:
	gcc -O4 -o infixlex.o -c infixlex.c

clean: 
	rm infixlex.c infixlex.o
