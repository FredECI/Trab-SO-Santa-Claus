#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_REINDEER 9  // Número de renas
#define NUM_ELVES 10    // Número de elfos

pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elfMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t reindeerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elfCond = PTHREAD_COND_INITIALIZER;

int numReindeerArrived = 0;
int numElvesWaiting = 0;

void *santaThread(void *arg) {
    while (1) {
        pthread_mutex_lock(&santaMutex);

        // Aguarda por três elfos ou três renas
        while (numReindeerArrived < 3 && numElvesWaiting < 3) {
            pthread_cond_wait(&reindeerCond, &santaMutex);
            pthread_cond_wait(&elfCond, &santaMutex);
        }

        // Lógica para lidar com os elfos e com as renas
        if (numReindeerArrived == 3) {
            printf("Papai Noel: Preparando o trenó\n");
            sleep(1);  // Simula preparação do trenó
            printf("Papai Noel: Entregando presentes\n");
            sleep(1);  // Simula entrega de presentes
            numReindeerArrived = 0;  // Reseta contador de renas
            printf("Papai Noel: Presentes entregues! Voltando para o Polo Norte\n");
            sleep(1);  // Simula retorno ao Polo Norte
            pthread_cond_broadcast(&reindeerCond);
        } else if (numElvesWaiting == 3) {
            printf("Papai Noel: Ajudando os elfos a resolverem problemas\n");
            sleep(1);  // Simula ajuda aos elfos
            numElvesWaiting = 0;  // Reseta contador de elfos
            pthread_cond_broadcast(&elfCond);
        }

        pthread_mutex_unlock(&santaMutex);
    }
    return NULL;
}

void *reindeerThread(void *arg) {
    // Lógica para que as renas retornem do Polo Norte
    printf("Rena: Voltando do Polo Norte\n");
    sleep(rand() % 3 + 1);  // Simula tempo de retorno das renas

    pthread_mutex_lock(&reindeerMutex);
    numReindeerArrived++;

    if (numReindeerArrived == 3) {
        printf("Rena: Última rena voltou! Acordando o Papai Noel\n");
        pthread_cond_signal(&reindeerCond);
    }

    pthread_cond_wait(&reindeerCond, &reindeerMutex); // Aguarda o Papai Noel liberar

    pthread_mutex_unlock(&reindeerMutex);

    // Lógica para as renas aguardarem pelo Papai Noel
    pthread_mutex_lock(&santaMutex);
    pthread_mutex_unlock(&santaMutex);

    return NULL;
}

void *elfThread(void *arg) {
    // Lógica para os elfos necessitem de ajuda
    printf("Elfo: Precisando de ajuda\n");
    sleep(rand() % 3 + 1);  // Simula tempo para precisar de ajuda

    pthread_mutex_lock(&elfMutex);
    numElvesWaiting++;

    if (numElvesWaiting == 3) {
        printf("Elfo: Três elfos precisando de ajuda! Acordando o Papai Noel\n");
        pthread_cond_signal(&elfCond);
    }

    pthread_cond_wait(&elfCond, &elfMutex); // Aguarda o Papai Noel liberar

    pthread_mutex_unlock(&elfMutex);

    // Lógica para os elfos aguardarem pelo Papai Noel
    pthread_mutex_lock(&santaMutex);
    pthread_mutex_unlock(&santaMutex);

    return NULL;
}

int main() {
    pthread_t santaTid;
    pthread_t reindeerTids[NUM_REINDEER];
    pthread_t elfTids[NUM_ELVES];

    // Criação das threads do Papai Noel, das renas e dos elfos
    pthread_create(&santaTid, NULL, santaThread, NULL);
    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_create(&reindeerTids[i], NULL, reindeerThread, NULL);
    }
    for (int i = 0; i < NUM_ELVES; i++) {
        pthread_create(&elfTids[i], NULL, elfThread, NULL);
    }

    // Aguarda todas as threads terminarem
    pthread_join(santaTid, NULL);
    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_join(reindeerTids[i], NULL);
    }
    for (int i = 0; i < NUM_ELVES; i++) {
        pthread_join(elfTids[i], NULL);
    }

    return 0;
}
