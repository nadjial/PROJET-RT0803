#ifndef NOEUD_H_
#define NOEUD_H_
#include <iostream>
#include <queue>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <map>

using namespace std;

//  VARIABLE GLOBAL
extern int run;
extern map<int,int> mst;
extern pthread_mutex_t mst_mutex;

class Noeud;

typedef struct canalsortie {
  int gauche;
  int droit;
  int poid;
}canalsortie;

extern map<int, canalsortie> touscanaux;
// basic=0, branch= 1, reject= 2
typedef struct canal {
  int etat;
  int poid;
  Noeud* noeud;
}canal;

typedef struct message {
  int id;
  int args[3];
  int poid;
}message;

// spleep= 0, find= 1, found= 2
class Noeud{
  public:
    Noeud();
    ~Noeud();
    int etat;
    canal *canaux;
    int ind;
    int nom;
    int numCanal;
    int niv;
    int mcan;
    int mpoids;
    int testcan;
    int pere;
    int recu;
    queue<message> messages;
    pthread_mutex_t mutex;
    void resultat();
    void init(int numCanal, int* poids, Noeud** noeuds);
    void ajoutMessage(message);
    void lireMessage();
    int  MinCanal();
    void BLOC1(); //initialisation
    void BLOC2(int, int); //connect
    void BLOC3(int, int, int, int); // Initiate
    void BLOC4();// Procedure test
    void BLOC5(int, int, int);// testmessage
    void BLOC6(int); //accept
    void BLOC7(int); // reject
    void BLOC8(); //report
    void BLOC9(int, int); // Reportmessage
    void BLOC10(int); //Changeroot
    void BLOC11(); //Changerootmessage
};
#endif // Noeud.h
