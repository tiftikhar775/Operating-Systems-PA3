#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cmath>

static pthread_mutex_t bsem;    // Mutex semaphore
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;  // Condition variable to control the turn
static int turn; // Index to control access to the turn array

struct infoFromMain
{
    int pos;
    int threadNumber;
};

void *bitToBase10(void *void_ptr_argv)
{
    infoFromMain myTurn = *(infoFromMain*) void_ptr_argv;
    pthread_mutex_lock(&bsem);
    while (myTurn.threadNumber != turn){
        pthread_cond_wait(&waitTurn,&bsem);
    }
    pthread_mutex_unlock(&bsem);
    std::cout << "2 ^ " << myTurn.pos << " = " << std::pow(2,myTurn.pos) <<std::endl;
    pthread_mutex_lock(&bsem);
    turn -= 1;
    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);
    return nullptr;
}

int main()
{
    int nThreads =0;
    std::string binNumber;
    std::cin >> binNumber;
    // Calculating the number of threads based on the input (only bits equal to 1)
    for (int i=0;i<binNumber.size();i++)
        if (binNumber[i] == '1')
            nThreads++;
    
    pthread_mutex_init(&bsem, NULL); // Initialize bsem to 1
 	
 	pthread_t *tid= new pthread_t[nThreads];
	infoFromMain *arg = new infoFromMain[nThreads];
	turn = nThreads-1; // initialize the turn here
	int tNumber = 0;
	for(int i=0;i<binNumber.size();i++)
	{
	    if (binNumber[i] == '1')  // Only create a thread if the bit is equal to 1
        {   arg[tNumber].threadNumber = tNumber; // initialize the thread number here (remember to follow the rules from the specifications of the assignment)
            arg[tNumber].pos = binNumber.size()-1-i;
            // call pthread_create
            pthread_create(&tid[tNumber],NULL,bitToBase10,&arg[tNumber]);
            
            tNumber++;
        }
	}
	// Call pthread_join
	for(int i = 0; i < nThreads; i++){
	    pthread_join(tid[i],NULL);
	}
	
    delete [] arg;
    delete [] tid;
	return 0;
}