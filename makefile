CC = g++ -pthread
OBJETS = noeud.o main.o

editor : $(OBJETS)
	$(CC) -o ghs $(OBJETS)

main.o: main.cpp noeud.h
	$(CC) -c main.cpp

node.o: noeud.cpp noeud.h
	$(CC) -c noeud.cpp

clean:
	rm ghs $(OBJETS) *~