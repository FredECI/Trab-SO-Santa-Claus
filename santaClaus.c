#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#define NUM_REINDEER 9             // Número de renas (fixo)
#define NUM_ELVES_TO_WAKE_UP 3    // Número de elfos necessários para acordar o Papai Noel


pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elfBlockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elfMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elfCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santaCond = PTHREAD_COND_INITIALIZER;
pthread_t *elvesTids;

int numReindeerArrived = 0;
int numElvesWaiting = 0;
int elves;

void *getSleight() {
    sleep(1); // Simula tempo de preapação do trenó
    return NULL;
}

void *getHitched() {
    sleep(1); // Simula prendimento das renas ao trenó
    return NULL;
}

void *santaThread(void *arg) {
    while (1) {
        pthread_mutex_lock(&santaMutex);
        
        // Wait for three elves or three reindeer while sleeping
        while (!(numReindeerArrived == NUM_REINDEER || numElvesWaiting == NUM_ELVES_TO_WAKE_UP)) {
            pthread_cond_wait(&santaCond, &santaMutex);
        }

        // Santa is now awake
        printf("PAPAI NOEL: Acordando\n");
        sleep(1);

        // Logic to handle the reindeer
        if (numReindeerArrived == NUM_REINDEER) {
            printf("PAPAI NOEL: Preparando o trenó\n");
            getSleight();
            printf("PAPAI NOEL: Chamando Renas\n");
            numReindeerArrived -= NUM_REINDEER;
            pthread_cond_broadcast(&reindeerCond);
            //Mata todos os elfos presos(Elfos presos = Elfos - Maior multiplo de 3 antes do número de elfos)
            for (int i = 0; i < elves; i++) {
                pthread_kill(elvesTids[i], SIGTERM);
            }
        }

        // Logic to handle the elves
        else if (numElvesWaiting == NUM_ELVES_TO_WAKE_UP) {
            printf("PAPAI NOEL: Ajudando os elfos a resolverem problemas\n");
            sleep(2); // Simulate helping the elves
            numElvesWaiting = 0; // Reset the elf count
            pthread_cond_broadcast(&elfCond);
        }
        pthread_mutex_unlock(&santaMutex);
    }
    return NULL;
}

void *reindeerThread(void *arg) {
    // Lógica para que as renas retornem do Polo Norte
    pthread_mutex_lock(&reindeerMutex);

    printf("RENA: Voltando do Polo Norte\n");
    sleep(1);  // Simula tempo de retorno das renas
    numReindeerArrived++;

    if (numReindeerArrived == NUM_REINDEER) {
        printf("RENA: Última rena voltou! Acordando o Papai Noel\n");
        pthread_cond_signal(&santaCond);
    }

    // Aguarda o Papai Noel liberar
    pthread_cond_wait(&reindeerCond, &reindeerMutex);
    printf("PAPAI NOEL: Liberou a Rena, prendendo-se ao trenó\n");
    getHitched();
    pthread_mutex_unlock(&reindeerMutex);
    return NULL; 
}


void *elfThread(void *arg) {
    // Lógica para os elfos necessitem de ajuda
    sleep(rand() % 3 + 1);  // Simula tempo para precisar de ajuda
    //Bloqueia todos os outros elfos
    pthread_mutex_lock(&elfBlockMutex);
    pthread_mutex_lock(&elfMutex);
    numElvesWaiting++;

    if (numElvesWaiting == 3) {
        printf("Elfo: Três elfos precisando de ajuda! Acordando o Papai Noel\n");
        pthread_cond_signal(&santaCond);
    }
    else{
        pthread_mutex_unlock(&elfBlockMutex);
        //se não for o último deixa outros elfos incrementarem
    }
    pthread_cond_wait(&elfCond, &elfMutex); // Aguarda o Papai Noel liberar
    pthread_mutex_unlock(&elfMutex);
    pthread_mutex_unlock(&elfBlockMutex);
    printf("Elfo terminou\n");
    return NULL;
}


int main() {
    pthread_t santaTid;
    pthread_t reindeerTids[NUM_REINDEER];
    printf("Entre com o número de elfos: \n");
    scanf("%i", &elves);
    elvesTids = (pthread_t *)malloc(elves * sizeof(pthread_t));
    
    // Criação das threads do Papai Noel, das renas e dos elfos
    printf("INICIANDO O PAPAI NOEL\n");
    pthread_create(&santaTid, NULL, santaThread, NULL);

    for (int i = 0; i < 3; i++) {
        
        printf("INICIANDO NOVA LEVA DE RENAS\n");
        for (int i = 0; i < NUM_REINDEER; i++) {
            reindeerTids[i] = i;
            pthread_create(&reindeerTids[i], NULL, reindeerThread, NULL);
        }
        printf("INICIANDO NOVA LEVA DE ELFOS\n");
        for (int i = 0; i < elves; i++) {
            elvesTids[i] = i + 10;
            pthread_create(&elvesTids[i], NULL, elfThread, NULL);
        }
        
        for (int i = 0; i < NUM_REINDEER; i++) {
            pthread_join(reindeerTids[i], NULL);
        }
        for (int i = 0; i < elves; i++) {
            pthread_join(elvesTids[i], NULL);
        }
    }

    // Não é elegante, mas forçamos o fim da thread do papai noel
    pthread_kill(santaTid, SIGTERM);
    pthread_join(santaTid, NULL);

    // printf("INICIANDO NOVA LEVA DE RENAS\n");
    // for (int i = 0; i < NUM_REINDEER; i++) {
    //     pthread_create(&reindeerTids[i], NULL, reindeerThread, NULL);
    // }
    // for (int i = 0; i < NUM_ELVES; i++) {
    //     pthread_create(&elfTids[i], NULL, elfThread, NULL);
    // }

    // Aguarda todas as threads terminarem
    // for (int i = 0; i < NUM_REINDEER; i++) {
    //     pthread_join(reindeerTids[i], NULL);
    // }
    // for (int i = 0; i < NUM_ELVES; i++) {
    //     pthread_join(elfTids[i], NULL);
    // }

    // Encerramento do Papai Noel

    return 0;
}
