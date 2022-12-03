#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

static pthread_mutex_t bsem;
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
static int turn;
static int nthreads;

//print ascending
void *print(void *void_ptr_argv) { 

    int myTurn = *(int *) void_ptr_argv;
    pthread_mutex_lock(&bsem);
    while(myTurn != turn){
       pthread_cond_wait(&waitTurn, &bsem);
    }
    pthread_mutex_unlock(&bsem);
        
    std::cout << "I am thread " << myTurn << std::endl;

    pthread_mutex_lock(&bsem);
    turn++;
    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);

    return NULL;
}

int main() {
    //int nthreads;
    std::cin >> nthreads;
    pthread_mutex_init(&bsem,NULL);
    pthread_t *tid = new pthread_t[nthreads];
    int *threadNumber = new int[nthreads];

    turn = 0;

    for(int i=0; i<nthreads; i++){
        threadNumber[i] = i;
        if(pthread_create(&tid[i], NULL, print, &threadNumber[i])){
            std::cerr << "Failed to create thread";
            return 1;
        }
    }
    //wait for other threads to finish
    for(int i=0; i<nthreads; i++){
        pthread_join(tid[i], NULL);
    }
    delete[] threadNumber;
    delete[] tid;
    return 0;
}