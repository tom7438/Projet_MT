#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define MAX_RUNS    1000
#define minimum(a,b)  ((a)>(b)?(b):(a))

#define TAILLE_MAX_MOTS 2048

unsigned char** domsu;
unsigned char** domsv;
int* doms_mask;

int doms_size=40;

int doms_lus=0;

void alloc_doms(int debut, int fin) {
   int i;
   for (i = debut; i < fin; i++) {
      domsu[i] = (unsigned char*)malloc(TAILLE_MAX_MOTS*sizeof(unsigned char));
      domsv[i] = (unsigned char*)malloc(TAILLE_MAX_MOTS*sizeof(unsigned char));
   }
}

void grow_doms_tables() {
   int oldsize = doms_size;
   doms_size *=2;
   domsu = (unsigned char**)realloc(domsu,sizeof(unsigned char*)*doms_size);
   domsv = (unsigned char**)realloc(domsv,sizeof(unsigned char*)*doms_size);
   doms_mask = (int*)realloc(doms_mask, sizeof(int)*doms_size);
   alloc_doms(oldsize,doms_size);
}


int input_dominos(const char* fichier) {
   int i;
   FILE* dom = NULL;
   dom = fopen(fichier, "r");
   if (!dom) {
      printf("Erreur a l'ouverture de %s\n", fichier);
      exit(1);
   }
   for (i = 0; fscanf(dom, "%s %s", domsu[i],domsv[i]) != EOF; i++)
      if (i > doms_size - 5) grow_doms_tables();
   fclose(dom);
   return i;
}

void print_dominos() {
   int i;
   for (i = 0; i<doms_lus; i++)
      if (doms_mask[i]) printf("%d:\t%s\n\t%s\n",i,domsu[i],domsv[i]);
}

int taille_mot = 50;

unsigned char* mot_haut;
unsigned char* mot_bas;

int pos_haut = 0;
int pos_bas = 0;


void grow_mots() {
   taille_mot *= 2;
   mot_haut = (unsigned char*)realloc(mot_haut,
                                      sizeof(unsigned char)*taille_mot);
   mot_bas = (unsigned char*)realloc(mot_bas,
                                     sizeof(unsigned char)*taille_mot);
}

void print_mots() {
   printf("%s\n%s\n", mot_haut, mot_bas);
}


int choose_domino() {
   int j, count, nonnul;
   int i = -1;
   count = 0;
   for (j=0; j < doms_lus; j++) {
      if (doms_mask[j] > 0) {
          nonnul  =j;
          count++;
      }
   }
   if (count == 1) {
      printf("Pas le choix, on prend:\n%s\n%s\n",
             domsu[nonnul],domsv[nonnul]);
      return nonnul;
   }
   if (count == 0) {
      printf("On ne peut pas continuer!\n");
      print_mots();
      exit(1);
   }
   printf("Nous en sommes a :\n");
   print_mots();
   printf("Veuillez choisir un domino :\n");
   print_dominos();
   scanf("%d", &i);
   if (i < 0 || i > doms_lus || doms_mask[i] == 0) return choose_domino();
   return i;
}


int try_domino(int dom) {
   int ret = 0;
   while (pos_haut + strlen((char*)domsu[dom]) > taille_mot - 5) grow_mots();
   while (pos_bas + strlen((char*)domsv[dom]) > taille_mot - 5) grow_mots();
   strcpy((char*)mot_haut + pos_haut, (char*)domsu[dom]);
   strcpy((char*)mot_bas + pos_bas, (char*)domsv[dom]);
   if (strncmp((char*)mot_haut, (char*)mot_bas,
               minimum(pos_haut + strlen((char*)domsu[dom]),
                       pos_bas + strlen((char*)domsv[dom]))) == 0) ret = 1;
   mot_haut[pos_haut] = 0;
   mot_bas[pos_bas]=0;
   return ret;
}


int find_domino() {
   int i = 0;
   for (i=0; i<doms_lus; i++) doms_mask[i] = try_domino(i);
   return choose_domino();
}


int is_not_finished() {
   if (pos_haut > 0 && pos_bas > 0 && pos_haut == pos_bas) return 0;
   else return 1;
}

void add_domino(int dom) {
   while (pos_haut + strlen((char*)domsu[dom]) > taille_mot - 5) grow_mots();
   while (pos_bas + strlen((char*)domsv[dom]) > taille_mot - 5) grow_mots();
   strcpy((char*)mot_haut + pos_haut, (char*)domsu[dom]);
   strcpy((char*)mot_bas + pos_bas, (char*)domsv[dom]);
   pos_haut += strlen((char*)domsu[dom]);
   pos_bas += strlen((char*)domsv[dom]);
}


int main(int argc, char* argv[]) {
   char* fichier;
   int i = 0;
   if (argc < 2) fichier = "dominos.txt";
   else fichier = argv[1];
   domsu = (unsigned char**)malloc(sizeof(unsigned char*) * doms_size);
   domsv = (unsigned char**)malloc(sizeof(unsigned char*) * doms_size);
   doms_mask = (int*)malloc(sizeof(int) * doms_size);
   mot_haut = (unsigned char*)malloc(sizeof(unsigned char) * taille_mot);
   mot_bas = (unsigned char*)malloc(sizeof(unsigned char) * taille_mot);
   alloc_doms(0, doms_size);
   doms_lus = input_dominos(fichier);
   while (is_not_finished() && (i++ < MAX_RUNS)) add_domino(find_domino());
   printf("Mot accepte !\n");
   print_mots();
   return 0;
}
