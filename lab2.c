/*Miguel Lima Tavares
 *DRE:119161571
 *Disciplina: Computacao Concorrente
 *Prof: Silvana Rossetto
 *Laboratorio 1
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

int **C;
int nThreads;

typedef struct{
    int id;
    int tamanho;
    int **A;
    int **B;
} tArgs;


void *tarefa(void *arg){
    tArgs *args = (tArgs*) arg;

    int tamanho = args->tamanho ;
    int id = args->id;
    int **A = args->A;
    int **B = args->B;

    clock_t tempoThread = clock();
    //pula as linhas da matriz de acordo com o numero de threads
    for (int i = id; i < tamanho; i += nThreads){
        for (int j = 0; j < tamanho; j++){
                    C[i][j] = 0;
                for (int k = 0; k < tamanho; k++){
                    C[i][j] += A[i][k] * B[k][j]; 
                }
        }
    }
    tempoThread = clock() - tempoThread;
    double tempoExecucao4 = (double)(tempoThread) / CLOCKS_PER_SEC;
    printf("\nTempo gasto thread %d: %f\n",id ,tempoExecucao4);

}

//--------------------------------------------------------------
// Libera memoria dos ponteiros para os calculos
int **liberarMemoria(int **vetor, int tamanho) {

    if (vetor == NULL) {
        return (NULL);
    }

    for (int i = 0; i < tamanho; i++) { 
        if (vetor[i]) { 
            free(vetor[i]); // libera linha da matriz
            vetor[i] = NULL;
        } 
    } 

    free(vetor);         //libera ponteiro
    vetor = NULL;

    return (NULL);   //retorna ponteiro null
}
//--------------------------------------------------------------

int **alocarMatrix(int tamanho) {


    int **vetor;
    
    //Alocacao de um vetor
    vetor = (int**) malloc(tamanho * sizeof(int*));

    if (vetor == NULL) {
        printf ("Erro ao alocar memoria");
        return (NULL);
    }

    // aloca memoria para cada vetor da matriz
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = (int*) malloc(tamanho * sizeof(int));

        if (vetor[i] == NULL) {
            printf ("Memoria insuficiente");
            liberarMemoria(vetor, tamanho);
            return (NULL);
        }

        // inicializa matriz com 0s
            for (int j = 0; j < tamanho; j++)
                vetor[i][j] = 0;
        
    }

    return (vetor);     // returns the pointer to the vector. 

}
//--------------------------------------------------------------

//Alocacao na memoria usando malloc
//inicia a matriz com valores aleatorios
//variavel tamanho matriz quadrada NxN
int **alocarMatrixInicial(int tamanho) {

    //srand(time(NULL));
    int **vetor;
    
    //Alocacao de um vetor
    vetor = (int**) malloc(tamanho * sizeof(int*));

    if (vetor == NULL) {
        printf ("Erro ao alocar memoria");
        return (NULL);
    }

    // aloca memoria para cada vetor da matriz
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = (int*) malloc(tamanho * sizeof(int));

        if (vetor[i] == NULL) {
            printf ("Memoria insuficiente");
            liberarMemoria(vetor, tamanho);
            return (NULL);
        }

        // inicializa matriz com 0s para a matriz tamanho
            for (int j = 0; j < tamanho; j++)
                vetor[i][j] = 0;
        

        // inicializa matriz com numeros entre -1000 e 1000
                for (int j = 0; j < tamanho; j++) {
                    if (i < tamanho)
                        vetor[i][j] = (rand() % (2 * 1001)) - 1001;
                }
        
    }

    return (vetor);     // retorna o ponteiro para o vetor 

}
//--------------------------------------------------------------


//Multiplicacao ingenua de duas matrizes
void multiplica(int **A, int **B, int **C, int tamanho) {
    pthread_t *tid;
    tArgs *args;

    tid = (pthread_t*) malloc(sizeof(pthread_t) * nThreads);
    if (tid == NULL)
        printf("Erro malloc threads\n");

    args = (tArgs*) malloc(sizeof(tArgs) * nThreads);
    if (args == NULL)
        printf("Erro malloc args\n");
    

    //gera threads
    clock_t tempoCriarThreads = clock();
    for (int i = 0; i < nThreads; i++){
        (args + i)->id = i;
        (args + i)->tamanho = tamanho;
        (args + i)->A = A; (args + i)->B = B;
        if (pthread_create(tid + i, NULL, tarefa, (void*) (args + i))){
            printf("Erro pthread_create\n");
            break;
        }
    }

    tempoCriarThreads = clock() - tempoCriarThreads;
    double tempoExecucao2 = (double)(tempoCriarThreads) / CLOCKS_PER_SEC;
    printf("\nTempo gasto criacao das threads %f\n", tempoExecucao2);

    //espera o fim das threads
    for (int i = 0; i < nThreads; i++)
        pthread_join(*(tid + i), NULL);
    
    free(args);
    free(tid);
}
//--------------------------------------------------------------

int main(void) {

    int **A;
    int **B;    
    int tamanho;

    printf("Digite o tamanho da matriz\n");
    scanf("%d",&tamanho);
    printf("Digite a quantidade de threads\n");
    scanf("%d",&nThreads);

    if (nThreads > tamanho)
        nThreads = tamanho;
    
    

    srand(time(NULL));
    clock_t tempoAlocar = clock();
    A = alocarMatrixInicial(tamanho);
    B = alocarMatrixInicial(tamanho);
    C = alocarMatrix(tamanho);
    tempoAlocar = clock() - tempoAlocar;
    double tempoExecucao1 = (double)(tempoAlocar) / CLOCKS_PER_SEC;
    printf("\nTempo gasto alocar vetores %f\n", tempoExecucao1);

    //-------------------------------------------------------
    //Imprime matrizes geradas aleatoriamente
    //Comentar para matrizes grandes
/*
    printf("\nMatrix A \n");
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++)
            printf("%d ", A[i][j]);
            printf("\n");
    }
    printf("\nMatrix B \n");
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++)
            printf("%d ", B[i][j]);
            printf("\n");
    }
*/
    //--------------------------------------------------------

    //Multiplicacao 
    clock_t tempoMultiplicar = clock();
    multiplica(A,B,C,tamanho);
    tempoMultiplicar = clock() - tempoMultiplicar;
    double tempoExecucao3 = (double)(tempoMultiplicar) / CLOCKS_PER_SEC;
    printf("\nTempo gasto multiplicacao %f\n", tempoExecucao3);
  /*  
    printf("Matrix resultante \n");
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++)
            printf("%d ", C[i][j]);
            printf("\n");
    }
*/
    
    

    A = liberarMemoria(A,tamanho);
    B = liberarMemoria(B,tamanho);
    C = liberarMemoria(C,tamanho);
}
