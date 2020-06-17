#include <iostream>
#include <pthread.h>
#include "noeud.h"
#include <cstdio>
#include <cstdlib>


using namespace std;

void *exec_thread(void *arg) {
  Noeud *noeud;
  noeud = (Noeud *)arg;
  while(run)
    noeud->lireMessage();
  return NULL;
}

int main(int argc, char *argv[]){ //(void){
  int i,j,k,rc,numNoeud,w,*poids;
  FILE *fp;

  if (argc < 2){
  std::cout<<"veuillez executer avec un fichier comme parametre d'entrée"<<endl;
  return -1;
    } 
  fp = fopen(argv[1],"r");
  fscanf(fp,"%d\n",&numNoeud);
  pthread_t *threads;
  Noeud *noeuds;
  Noeud *nbrs[numNoeud];
  threads = (pthread_t *)malloc(numNoeud*sizeof(pthread_t));
  noeuds = new Noeud[numNoeud];
  poids = (int *)malloc(numNoeud*sizeof(int));
  for(i=0;i<numNoeud;i++){
    k=0;
    for(j=0;j<numNoeud;j++){
      fscanf(fp,"%d\t",&w);
      if(w!=0){
        poids[k]=w;
        nbrs[k]=&noeuds[j];
        k++;

        if(j<i){
          canalsortie e;
          e.gauche = j;
          e.droit= i;
          touscanaux.insert(pair<int,canalsortie>(w,e));
        }
      }
    }
    fscanf(fp,"\n");
    noeuds[i].init(k,poids,nbrs);
    noeuds[i].ind=i;
  }
  fclose(fp);
  run = 1;
  for (i=0;i<numNoeud;i++){
    rc = pthread_create(&threads[i],NULL,exec_thread,(void *) &noeuds[i]);
  }
  noeuds[0].BLOC1();

  pthread_exit(NULL);
  free(noeuds);
  free(threads);
  free(poids);
  free(nbrs);
  return 0;
}