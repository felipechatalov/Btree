#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAVES 5

typedef struct {
    int RAIZ;
} CABECALHO;
// 4 bytes

typedef struct {
    int CONTACHAVES;    // numero de chaves alocadas na pagina -- maximo de "MAXCHAVES"
    int CHAVES[MAXCHAVES-1];     // valor das chaves armazenadas na pagina
    int FILHOS[MAXCHAVES];    // vetor com os filhos/rrn dos filhos apontando para a proxima pagina
} PAGINA;
// 4
// 4 * 4 = 16
// 5 * 4 = 20
// 40 bytes

typedef struct
{
  int rrn_raiz;
  int qtd_paginas;
} CABECA;



PAGINA lerPagina(int rrn, FILE * file){
    PAGINA temp;
    fseek(file, rrn*sizeof(PAGINA) + sizeof(CABECALHO), SEEK_SET);
    fread(&temp, sizeof(PAGINA), 1, file);
    return temp;
}

int getRrnNewPage(FILE* file){
    fseek(file, 0, SEEK_END);
    return (ftell(file)-sizeof(CABECALHO))/sizeof(PAGINA);
}

int buscaNaPagina(int chave, PAGINA* pag, int* pos){
    int i = 0;
    while(i < pag->CONTACHAVES && chave > pag->CHAVES[i]){
        i++;
    }
    *pos = i;
    if (*pos < pag -> CONTACHAVES && chave == pag->CHAVES[*pos]){
        return 1;
    }
    else{
        return 0;
    }
}

PAGINA inicializaPagina(PAGINA pag){
    pag.CONTACHAVES = 0;
    for (int i = 0; i < MAXCHAVES; i++){
        pag.CHAVES[i] = -1;
        pag.FILHOS[i] = -1;
    }
    pag.FILHOS[MAXCHAVES] = -1;
    return pag;
}

int escrevePagina(int rrn, PAGINA* pag, FILE* file){
    fseek(file, rrn*sizeof(PAGINA)+sizeof(CABECALHO), SEEK_SET);
    fwrite(pag, sizeof(PAGINA), 1, file);

}

int printPagina(PAGINA* pag){
    int i, j;
    // printf("\nContaChaves -> %d", pag -> CONTACHAVES);
    printf("\nChaves:");
    for(i = 0; i < pag->CONTACHAVES-1; i++){
        printf(" %d |", pag->CHAVES[i]);
    }
    printf(" %d ", pag->CHAVES[i]);



    printf("\nFilhos:");
    for(j = 0; j < pag->CONTACHAVES; j++){
        printf(" %d |", pag->FILHOS[j]);
    }
    printf(" %d ", pag->FILHOS[j]);

}

int printChaves(PAGINA* pag){
    int i, j;
    int aux=0;

     for(i = 0; i < pag->CONTACHAVES-1; i++){
        printf(" %d |", pag->CHAVES[i]);
    }
      printf("%d ", pag->CHAVES[i]);
      //printf("I --> %i", i);
     // printf(" %d ", pag->CHAVES[i]);
      //vet[i] = pag->CHAVES[i];
     
}

int logPagina(FILE* file, PAGINA* pag){
    int i, j;
    fprintf(file, "\nContaChaves -> %d", pag -> CONTACHAVES);
    fprintf(file, "\nChaves:");
    for(i = 0; i < pag->CONTACHAVES-1; i++){
        fprintf(file, " %d |", pag->CHAVES[i]);
    }
    fprintf(file, " %d ", pag->CHAVES[i]);



    fprintf(file, "\nFilhos:");
    for(j = 0; j < pag->CONTACHAVES; j++){
        fprintf(file, " %d |", pag->FILHOS[j]);
    }
    fprintf(file, " %d ", pag->FILHOS[j]);

}
// 0 -> NAO_ENCONTRADO
// 1 -> ENCONTRADO
int busca(int rrn, int chave, FILE* file, int* rrn_encontrado, int* pos_encontrado){
    PAGINA pag_temp;
    int encontrado, pos;

    if (rrn == -1){
        return 0;
    }
    pag_temp = lerPagina(rrn, file);
    encontrado = buscaNaPagina(chave, &pag_temp, &pos);

    if (encontrado){
        *rrn_encontrado = rrn;
        *pos_encontrado = pos;
        return 1;
    }
    else{
        return busca(pag_temp.FILHOS[pos], chave, file, rrn_encontrado, pos_encontrado);
    }

}

PAGINA insereNaPagina(int chave, int filho_d, PAGINA* pag){
    int i = pag -> CONTACHAVES;
    while( i > 0 && chave < pag -> CHAVES[i-1]){
        pag -> CHAVES[i] = pag -> CHAVES[i-1];
        pag -> FILHOS[i+1] = pag -> FILHOS[i];
        i--;
    }
    pag -> CONTACHAVES++;
    pag -> CHAVES[i] = chave;
    pag -> FILHOS[i+1] = filho_d;
    
}


PAGINA divideAntigo(int chave, int filho_d, PAGINA* pag, FILE* file, int * chave_pro, int* filho_d_pro, PAGINA* pag2){
    PAGINA pag_aux;
    pag_aux = *pag;

    int i = MAXCHAVES-1;
    

    // ciar 2 vetores adicionais auxiliares com 5 posicoes para chaves
    // e 6 posicoes para os filhos, trabalhar com eles ao inves
    // do objeto pag_aux, e por final separar corretamente qual ira
    // para o *pag e qual ira para o *pag2


    while(i > 0 && chave < pag_aux.CHAVES[i-1]){
        pag_aux.CHAVES[i] = pag_aux.CHAVES[i-1];
        pag_aux.FILHOS[i+1] = pag_aux.FILHOS[i];
        i--;
    }
    pag_aux.CHAVES[i] = chave;
    pag_aux.FILHOS[i+1] = filho_d;

    int mid = (MAXCHAVES-1)/2;

    *filho_d_pro = getRrnNewPage(file);
    *chave_pro = pag_aux.CHAVES[mid];


    *pag = inicializaPagina(*pag);

    i = 0;
    while(i < mid){
        pag->CHAVES[i] = pag_aux.CHAVES[i];
        pag->FILHOS[i] = pag_aux.FILHOS[i];
        pag->CONTACHAVES++;
        i++;
    }
    // chaves 0 e 1 para esq
    // filhos 0 1 e 2 para esq
    
    // filho 2 nao pode ir para esq e dir ao mesmo tempo
    
    // pag->FILHOS[i] = pag_aux.FILHOS[i];

    *pag2 = inicializaPagina(*pag2);

    i = mid;
    while (i < MAXCHAVES-1){
        pag2->CHAVES[pag2->CONTACHAVES] = pag_aux.CHAVES[i];
        pag2->FILHOS[pag2->CONTACHAVES] = pag_aux.FILHOS[i];
        pag2->CONTACHAVES++;
        i++;
    }
    // chaves 2 e 3 para esq
    // filhos 2 3 e 4 para filhos
    pag2->FILHOS[pag2->CONTACHAVES] = pag_aux.FILHOS[i];

}

PAGINA divide(int chave, int filho_d, PAGINA* pag, FILE* file, int * chave_pro, int* filho_d_pro, PAGINA* pag2){
    PAGINA pag_aux;
    pag_aux = *pag;

    
    int auxCONTACHAVES = pag_aux.CONTACHAVES;
    int auxCHAVES[MAXCHAVES];
    int auxFILHOS[MAXCHAVES+1];

    for (int k = 0; k < MAXCHAVES; k++){
        auxCHAVES[k] = pag->CHAVES[k];
        auxFILHOS[k] = pag->FILHOS[k];
    }
    auxCHAVES[MAXCHAVES-1] = -1;
    // auxFILHOS[MAXCHAVES] = -1;
    auxFILHOS[MAXCHAVES] = -1;
    


    int i = MAXCHAVES-1;

    while(i > 0 && chave < auxCHAVES[i-1]){
        auxCHAVES[i] = auxCHAVES[i-1];
        auxFILHOS[i+1] = auxFILHOS[i];
        i--;
    }
    auxCHAVES[i] = chave;
    auxFILHOS[i+1] = filho_d;


    int mid = (MAXCHAVES-1)/2;

    *filho_d_pro = getRrnNewPage(file);
    *chave_pro = auxCHAVES[mid];


    *pag = inicializaPagina(*pag);

    i = 0;
    while(i < mid){
        pag->CHAVES[i] = auxCHAVES[i];
        pag->FILHOS[i] = auxFILHOS[i];
        pag->CONTACHAVES++;
        i++;
    }
    pag->FILHOS[i] = auxFILHOS[i];
    //chaves 0 1
    //filhos 0 1 2

    *pag2 = inicializaPagina(*pag2);

    i = mid+1;
    while (i < MAXCHAVES){
        pag2->CHAVES[pag2->CONTACHAVES] = auxCHAVES[i];
        pag2->FILHOS[pag2->CONTACHAVES] = auxFILHOS[i];
        pag2->CONTACHAVES++;
        i++;
    }
   pag2->FILHOS[pag2->CONTACHAVES] = auxFILHOS[i];
    //chaves 3 4
    //fihlos 3 4 5


}






// 0 = erro
// 1 = sem promocao
// 2 = promocao
int insere(int rrn, int chave, FILE* file, int* filho_d_pro, int* chave_pro){
    PAGINA pag_temp;
    int temp, encontrado, pos;
    printf("b");
    if (rrn == -1){
        *chave_pro = chave;
        *filho_d_pro = -1;
        return 2;
    }
    pag_temp = lerPagina(rrn, file);
    encontrado = buscaNaPagina(chave, &pag_temp, &pos);
    
    if (encontrado){
        printf("\nChave duplicada");
        return 0;
    }
    int rrn_pro, chv_pro;
    temp = insere(pag_temp.FILHOS[pos], chave, file, &rrn_pro, &chv_pro);

    if (temp == 1 || temp == 0){
        return temp;
    }
    
    if (pag_temp.CONTACHAVES < MAXCHAVES-1){
        insereNaPagina(chv_pro, rrn_pro, &pag_temp);
        escrevePagina(rrn, &pag_temp, file);
        return 1;
    }
    else{
        PAGINA pag2;
        divide(chv_pro, rrn_pro, &pag_temp, file, chave_pro, filho_d_pro, &pag2);
        escrevePagina(rrn, &pag_temp, file);
        escrevePagina(*filho_d_pro, &pag2, file);
        return 2;
    }
}


int lerProximoInput(FILE* file){
    char buffer[8]; // aguenta de 0 ate 9999
    memset(buffer, 0, sizeof(buffer));


    int i = 0;
    int c = fgetc(file);
    while(c != EOF && c != '|'){
        
        // printf("\n char -> %c", c, c);
        buffer[i] = c;
        // printf("\n string -> %s", buffer);
        c = fgetc(file);

        i++;
    }
    // printf("\nint -> %d", atoi(buffer));
    if (c == EOF){
        return -1;
    }
    return atoi(buffer);
}

int imprimebtree(){
    FILE* file;
    PAGINA pag;
    int limit, i, raiz;



    if ((file = fopen("btree.dat", "rb")) == NULL){
        printf("Erro na abertura do arquivo 'btree.dat' \n");
        exit(EXIT_FAILURE);
    }

    fread(&raiz, sizeof(int), 1, file);


    fseek(file, 0, SEEK_END);
    limit = (ftell(file)-sizeof(CABECALHO))/sizeof(PAGINA);
    rewind(file);
    
    printf("a");

    i = 0;
    while(i < limit){
        if (i == raiz){
            pag = lerPagina(i, file);
            printf("\n- - - - Pagina Raiz - - - -");
            printf("\nPagina %d", i);
            printPagina(&pag);
            printf("\n- - - - - - - - - - - - - -");
            printf("\n");
            i++;
        }
        else{
            pag = lerPagina(i, file);
            printf("\nPagina %d", i);
            printPagina(&pag);
            printf("\n");
            i++;
        }
    }
}

    
int emOrdem(FILE *file, int rrn)
{
  PAGINA pag;
  if (rrn == -1)
    return 0;
  else
    pag = lerPagina(rrn, file);
  
  for (int i = 0; i <= pag.CONTACHAVES; i++)
  {
    emOrdem(file, pag.FILHOS[i]);
    if (pag.CHAVES[i] != -1)
      printf(" %d |", pag.CHAVES[i]);
  }
}
 

int logEverything(FILE* file, FILE* log){
    PAGINA pag;
    int limit, i;

    fseek(file, 0, SEEK_END);
    limit = (ftell(file)-sizeof(CABECALHO))/sizeof(PAGINA);
    rewind(file);
    

    i = 0;
    while(i < limit){
        pag = lerPagina(i, file);
        fprintf(log, "\nPagina %d", i);
        logPagina(log, &pag);
        i++;
    }
    fprintf(log, "\n\n\n\n");

}

int gerenciador(char* nome){
    FILE* arqLer, *arqEsc, *log;
    CABECALHO cab;
    if ((arqLer = fopen(nome, "rb")) == NULL) {
        printf("Erro na leitura do arquivo \n");
        exit(EXIT_FAILURE);
    }
    if ((arqEsc = fopen("btree.dat", "w+b")) == NULL){
        printf("Erro na criacao do arquivo \n");
        exit(EXIT_FAILURE);
    }
    if ((log = fopen("log.dat", "w")) == NULL){
        printf("Erro na criacao do arquivo \n");
        exit(EXIT_FAILURE);
    }

    cab.RAIZ = 0;
    fwrite(&cab, sizeof(CABECALHO), 1, arqEsc);

    PAGINA pag;
    pag = inicializaPagina(pag);
    escrevePagina(cab.RAIZ, &pag, arqEsc);

    int filho_d_pro = -1, chave_pro;
    int c = lerProximoInput(arqLer);
    
    while(c != -1){
        printf("\ninserindo '%d'", c);
        if (insere(cab.RAIZ, c, arqEsc, &filho_d_pro, &chave_pro) == 2){
            printf("\npromover");
            printf("\n chave_pro -> %d, cab.RAIZ -> %d, filho_d_pro -> %d", chave_pro, cab.RAIZ, filho_d_pro);
            PAGINA pagx = inicializaPagina(pagx);
            pagx.CHAVES[0] = chave_pro;
            pagx.CONTACHAVES++;
            pagx.FILHOS[0] = cab.RAIZ;
            pagx.FILHOS[1] = filho_d_pro;
            int novo_rrn = getRrnNewPage(arqEsc);
            escrevePagina(novo_rrn, &pagx, arqEsc);
            cab.RAIZ = novo_rrn;
        }
       
        fprintf(log, "\ninserindo %d\n", c);
        logEverything(arqEsc, log);
        // PAGINA p = lerPagina(0, arqEsc);
        // printPagina(&p);
        // if (c == 12){
        //     p = lerPagina(1, arqEsc);
        //     printPagina(&p);
        //     p = lerPagina(2, arqEsc);
        //     printPagina(&p);
        // }
        c = lerProximoInput(arqLer);
    }
    rewind(arqEsc);
    fwrite(&cab, sizeof(CABECALHO), 1, arqEsc);
    printf("\ntodas as paginas");
    // for (int j = 0; j < 9; j++){
    //     PAGINA p = lerPagina(j, arqEsc);
    //     printPagina(&p);
    // }

    fclose(arqEsc);

}

int criacao(char * nome){
    FILE* arqLer, *arqEsc;
    PAGINA pag0, pag1;
    CABECALHO cab;
    // if ((arqLer = fopen(nome, "rb")) == NULL) {
    //     printf("Erro na leitura do arquivo \n");
    //     exit(EXIT_FAILURE);
    // }
    if ((arqEsc = fopen("btree.dat", "w+b")) == NULL){
        printf("Erro na criacao do arquivo \n");
        exit(EXIT_FAILURE);
    }

    cab.RAIZ = 0;
    


    pag0.CONTACHAVES = 4;
    
    pag0.CHAVES[0] = 5; 
    pag0.CHAVES[1] = 6; 
    pag0.CHAVES[2] = 7; 
    pag0.CHAVES[3] = 8; 

    pag0.FILHOS[0] = 1;
    pag0.FILHOS[1] = -1;
    pag0.FILHOS[2] = -1;
    pag0.FILHOS[3] = -1;
    pag0.FILHOS[4] = -1;

    pag1.CONTACHAVES = 4;
    
    pag1.CHAVES[0] = 1; 
    pag1.CHAVES[1] = 2; 
    pag1.CHAVES[2] = 3; 
    pag1.CHAVES[3] = 4; 

    pag1.FILHOS[0] = -1;
    pag1.FILHOS[1] = -1;
    pag1.FILHOS[2] = -1;
    pag1.FILHOS[3] = -1;
    pag1.FILHOS[4] = -1;


    cab.RAIZ = 0;

    fwrite(&cab, sizeof(CABECALHO), 1, arqEsc);

    fwrite(&pag0, sizeof(PAGINA), 1, arqEsc);
    fwrite(&pag1, sizeof(PAGINA), 1, arqEsc);

    PAGINA buffer;
    buffer = lerPagina(0, arqEsc);
    // rewind(arqEsc);
    // fread(&buffer, sizeof(PAGINA), 1, arqEsc);


    printf("\nchaves -> %d", buffer.CONTACHAVES);
    printf("\nvalor -> %d", buffer.CHAVES[3]);
    printf("\nfilhos -> %d", buffer.FILHOS[3]);

    int rrn, pos;

    if (busca(cab.RAIZ, 7, arqEsc, &rrn, &pos) == 1){
        printf("\n rrn -> %d, pos -> %d", rrn, pos);
    }
    else{
        printf("\nNao encontrou");
    }

    fclose(arqEsc);

}   






int main (int argc, char *argv[]){

    if (argc == 3 && strcmp(argv[1], "-c" ) == 0){
        printf("Criacao --- nome do arquivo = %s\n", argv[2]);
        gerenciador(argv[2]);
    }

    else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Impressao das info '-p'\n");
        imprimebtree();
    }

    else if (argc == 2 && strcmp(argv[1], "-k") == 0) {
         printf("Impressao das chaves '-k'\n");
         FILE *file;
         CABECALHO cabeca;

         if (file = fopen("btree.dat", "r+b"))
         {
          fread(&cabeca, sizeof(int), 1, file);
          emOrdem(file, cabeca.RAIZ);
        }
    }
    
    else {
        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s -c 'nome_do_arquivo'\n", argv[0]);
        fprintf(stderr, "$ %s (-p|-k) \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}