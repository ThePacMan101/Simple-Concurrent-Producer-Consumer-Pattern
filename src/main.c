#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
typedef short int bool;
#define True  1
#define False 0


long int soma = 0;
bool ja_printei = False;
short int qtd_funcionando = 0 ;
pthread_mutex_t mutex;
pthread_cond_t pode_contar, pode_printar;

int nthreads;

void *ExecutaTarefa(void *arg){
    long int id = (long int)arg;
    printf("Thread : %ld esta executando...\n", id);

    for (int i = 0; i < 100000; i++){

        pthread_mutex_lock(&mutex);

        while (!ja_printei)
            pthread_cond_wait(&pode_contar,&mutex);
        
        soma++;
        if(soma%1000 == 0){
            ja_printei = False;
            pthread_cond_signal(&pode_printar);
        }
        
        pthread_cond_signal(&pode_printar);
        
        pthread_mutex_unlock(&mutex);

    }
    printf("Thread : %ld terminou!\n", id);
    qtd_funcionando --;
    if(qtd_funcionando == 0){
        pthread_cond_signal(&pode_printar);
    }
    pthread_exit(NULL);
}

void *extra(void *args)
{
    printf("Extra : esta executando...\n");
    while(!ja_printei || qtd_funcionando){
        pthread_mutex_lock(&mutex);
        
        while (ja_printei)
            pthread_cond_wait(&pode_printar,&mutex);
        
        printf("soma = %ld \n", soma);
        ja_printei = True;

        pthread_cond_broadcast(&pode_contar);
        
        pthread_mutex_unlock(&mutex);
    }
    printf("Extra : terminou!\n");
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t *tid;

    if (argc < 2){
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);
    qtd_funcionando = nthreads;

    tid = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
    if (tid == NULL){
        puts("ERRO--malloc");
        return 2;
    }

    pthread_mutex_init( &mutex        , NULL );
    pthread_cond_init ( &pode_printar , NULL );
    pthread_cond_init ( &pode_contar  , NULL );

    for (long int t = 0; t < nthreads; t++){
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)){
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    if (pthread_create(&tid[nthreads], NULL, extra, NULL)){
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }

    for (int t = 0; t < nthreads + 1; t++){
        if (pthread_join(tid[t], NULL)){
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    pthread_mutex_destroy(&mutex);

    printf("Valor de 'soma' = %ld\n", soma);

    return 0;
}
