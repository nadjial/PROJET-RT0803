#include <pthread.h>
#include "noeud.h"

#define INF 1000000
 int run;
 map<int,int> mst;
 pthread_mutex_t mst_mutex;
 map<int, canalsortie> touscanaux;



void Noeud::init(int numCanal, int poids[], Noeud* noeuds[]) {
  this->numCanal = numCanal;
  this->canaux = (canal *)malloc(sizeof(canal) * numCanal);
  for (int i = 0; i < numCanal; ++i) {
    this->canaux[i].etat = 0;
    this->canaux[i].poid = poids[i];
    this->canaux[i].noeud = noeuds[i];
  }
  this->etat = 0;
  this->mcan = -1;
  this->mpoids = INF;
  this->testcan = -1;
  this->pere= -1;
  this->niv = -1;
  this->recu = -1;
  this->nom = INF;
  this->mutex = PTHREAD_MUTEX_INITIALIZER;
}

Noeud::Noeud() {}

Noeud::~Noeud() {
  delete(this->canaux);
}

void Noeud::ajoutMessage(message msg) {
  pthread_mutex_lock(&(this->mutex));
  this->messages.push(msg);
  pthread_mutex_unlock(&(this->mutex));
}

void Noeud::lireMessage() {
  if (!this->messages.empty()) {
    pthread_mutex_lock(&(this->mutex));
    message msg = this->messages.front();
    this->messages.pop();
    int i;
    for (i = 0; i < this->numCanal; ++i) {
      if (this->canaux[i].poid == msg.poid)
        break;
    }
    pthread_mutex_unlock(&(this->mutex));
    if(this->etat == 0)
      this->BLOC1();

    switch(msg.id) {
      case 0:
        BLOC2(msg.args[0], i);
        break;
      case 1:
        BLOC3(msg.args[0], msg.args[1], msg.args[2], i);
        break;
      case 2:
        BLOC5(msg.args[0], msg.args[1], i);
        break;
      case 3:
        BLOC6(i);
        break;
      case 4:
        BLOC7(i);
        break;
      case 5:
        BLOC9(msg.args[0], i);
        break;
      case 6:
        BLOC10(i);
        break;
      case 7:
        this->BLOC1();
    }
  }
}

int Noeud::MinCanal() {
  int min = INF, index = 0;
  for (int i = 0; i < numCanal; i++) {
    if (this->canaux[i].poid < min) {
      min = this->canaux[i].poid;
      index = i;
    }
  }
  return index;
}

void Noeud::BLOC1() {
  int mindecanal= MinCanal();
  // Branch state
  this->canaux[mindecanal].etat = 1;
  pthread_mutex_lock(&(mst_mutex));
  mst.insert(pair<int,int>(this->canaux[mindecanal].poid,1));
  pthread_mutex_unlock(&(mst_mutex));
  this->niv = 0;
  // etat found
  this->etat = 2;
  this->recu= 0;
  message msg;
  msg.id = 0;
  msg.args[0] = 0;
  msg.poid = this->canaux[mindecanal].poid;
  this->canaux[mindecanal].noeud->ajoutMessage(msg);
}

void Noeud::BLOC2(int L, int j) {
  if (L < this->niv) {
    // etat branch
    this->canaux[j].etat = 1;
    message msg;
    msg.id = 1;
    msg.args[0] = this->niv;
    msg.args[1] = this->nom;
    msg.args[2] = this->etat;
    msg.poid = this->canaux[j].poid;
    this->canaux[j].noeud->ajoutMessage(msg);
  }
  else if (this->canaux[j].etat == 0) {
    message msg;
    msg.id = 0; // Connect Message
    msg.args[0] = L;
    msg.poid = this->canaux[j].poid;
    this->ajoutMessage(msg);
  }
  else {
    message msg;
    msg.id = 1; // Initiate Message
    msg.args[0] = this->niv + 1;
    msg.args[1] = this->canaux[j].poid;
    msg.args[2] = 1; // etat find
    msg.poid = this->canaux[j].poid;
    this->canaux[j].noeud->ajoutMessage(msg);
  }
}

void Noeud::BLOC3(int L, int id, int etatdenoeud, int j) {
  this->niv = L; // LN <- L
  this->nom = id; // FN <- F
  this->etat = etatdenoeud; // SN <- S
  this->pere = j; // in-branch <- j
  this->mcan = -1; // mcan<- nil
  this->mpoids = INF; // mpoids <- infinity
  for (int i = 0; i < numCanal; ++i) {
    if (i != j && this->canaux[i].etat == 1) {
      message msg;
      msg.id = 1; // Initiate Message
      msg.args[0] = L;
      msg.args[1] = id;
      msg.args[2] = etatdenoeud; // find etat
      msg.poid = this->canaux[i].poid;
      this->canaux[i].noeud->ajoutMessage(msg);
    }
  }
  if ( etatdenoeud== 1) {
    this->recu = 0;
    BLOC4();
  }
}

void Noeud::BLOC4() {
  int i, min, min_ind;
  min = INF;
  min_ind = -1;
  for (i = 0; i < numCanal; i++) {
    if (this->canaux[i].etat == 0) {
      if(min>this->canaux[i].poid){
        min = this->canaux[i].poid;
        min_ind = i;
      }
    }
  }
  if(min_ind>=0){
    this->testcan = min_ind;
    message msg;
    msg.id = 2; // Test Message
    msg.args[0] = this->niv;
    msg.args[1] = this->nom;
    msg.poid = this->canaux[min_ind].poid;
    this->canaux[min_ind].noeud->ajoutMessage(msg);
  }
  else{
    this->testcan = -1;
    BLOC8();
  }
}

void Noeud::BLOC5(int L, int id, int j) {
  if (L > this->niv) {
    message msg;
    msg.id = 2; // Test Message
    msg.args[0] = L;
    msg.args[1] = id;
    msg.poid = this->canaux[j].poid;
    this->ajoutMessage(msg);
  } 
  else if (id == this->nom) {
    if (this->canaux[j].etat == 0)
      this->canaux[j].etat = 2;
    if (j != this->testcan) {
      message msg;
      msg.id = 4; // Reject Message
      msg.poid = this->canaux[j].poid;
      this->canaux[j].noeud->ajoutMessage(msg);
    }
    else
      BLOC4();
  }
  else{
    message msg;
    msg.id = 3; // Accept Message
    msg.poid = this->canaux[j].poid;
    this->canaux[j].noeud->ajoutMessage(msg);
  }
}

void Noeud::BLOC6(int j) {
  this->testcan = -1;
  if(this->canaux[j].poid<this->mpoids){
    this->mcan = j;
    this->mpoids = this->canaux[j].poid;
  }
  this->BLOC8();
}

void Noeud::BLOC7(int j) {
  if(this->canaux[j].etat == 0)
    this->canaux[j].etat = 2;
  this->BLOC4();
}

void Noeud::BLOC8() {
  int i,k;
  k=0;
  for(i=0;i<this->numCanal;i++){
    if(this->canaux[i].etat==1 && i!=this->pere)
      k++;
  }
  if(recu == k && this->testcan == -1){
    this->etat = 2;
    message msg;
    msg.id=5;
    msg.args[0]=this->mpoids;
    msg.poid = this->canaux[this->pere].poid;
    this->canaux[this->pere].noeud->ajoutMessage(msg);
  }
}

void quicksort(vector<int> &dist, vector<int> &ind, int low, int high) {
  int i = low;
  int j = high;
  int z = dist[(low + high) / 2];
  do {
    while(dist[i] < z) i++;

    while(dist[j] > z) j--;

    if(i <= j) {
      /* swap two elements */
      int t = dist[i];
      int ti = ind[i];
      dist[i]=dist[j];
      dist[j]=t;

      ind[i] = ind[j];
      ind[j]=ti;
      i++; 
      j--;
    }
  } while(i <= j);

  if(low < j) 
    quicksort(dist,ind, low, j);

  if(i < high) 
    quicksort(dist,ind, i, high); 
}
void Noeud::resultat(){
  FILE *fp;
  vector<int> n1;
  vector<int> n2;
  vector<int> ind;
  int i;
  i = 0;
  for(map<int,int>::iterator it = mst.begin(); it != mst.end(); ++it){
    n1.push_back(touscanaux[it->first].gauche);
    n2.push_back(touscanaux[it->first].droit);
    ind.push_back(i++);
  }
  quicksort(n1,ind,0,n1.size()-1);
  fp = fopen("Resultat","w");
  for(i=0;i<n1.size();i++){
    fprintf(fp,"%d->%d\n",n1[i],n2[ind[i]]);
  }
  fclose(fp);
}


void Noeud::BLOC9(int poid, int j) {
  if(j != this->pere){
    if(poid < this->mpoids){
      this->mcan = j;
      this->mpoids = poid;
    }
    this->recu += 1;
    BLOC8();
  }
  else{ 
    if(this->etat == 1){
      message msg;
      msg.id = 5;
      msg.args[0]=poid;
      msg.poid = this->canaux[j].poid;
      //place recieved message at queue end
      this->ajoutMessage(msg);
    }
    else if(poid > this->mpoids){
      BLOC11();
    }
    else if (poid == INF && this->mpoids == INF){
      resultat();
      run = 0;
      //pthread_exit(NULL);
    }
  }
}	
void Noeud::BLOC11() {
  message msg;
  if(this->canaux[this->mcan].etat == 1){
    msg.id=6;
    msg.poid = this->canaux[this->mcan].poid;
    this->canaux[this->mcan].noeud->ajoutMessage(msg);
  }
  else{
    msg.id=0;
    msg.args[0]=this->niv;
    msg.poid = this->canaux[this->mcan].poid;
    this->canaux[this->mcan].noeud->ajoutMessage(msg);
    this->canaux[this->mcan].etat = 1;
    pthread_mutex_lock(&(mst_mutex));
    mst.insert(pair<int,int>(this->canaux[this->mcan].poid,1));
    pthread_mutex_unlock(&(mst_mutex));
  }
}

void Noeud::BLOC10(int j) {
  BLOC11();
}
