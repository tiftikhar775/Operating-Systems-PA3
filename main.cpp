#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct
{
    size_t bits;
    size_t n;
} ThreadReturn;

// function using pointers to get significant bits
ThreadReturn *getMostSignificantBits(double d, size_t n)
{
    ThreadReturn *result = (ThreadReturn *)calloc(1, sizeof(ThreadReturn));
    result->n = n;
    for (size_t i = 1; i <= n; ++i)
    {
        double p = 1.0 / (1 << i);
        if (d >= p)
        {
            d -= p;
            result->bits |= 1;
        }
        result->bits <<= 1;
    }
    return result;
}

typedef struct
{
    size_t i;
    double const *probs;
    pthread_mutex_t lock; // to synchronize the threads
} ThreadArg;

// function to encode input file
void *encode(void *arg)
{
    ThreadArg *targ = (ThreadArg *)arg;
    pthread_mutex_lock(&(targ->lock)); // acquiring the code before entering the critical section
    double F = targ->probs[targ->i] / 2;
    for (size_t j = 0; j < targ->i; ++j)
    {
        F += targ->probs[j];
    }
    pthread_mutex_unlock(&(targ->lock)); // releasing the lock

    return (void *)getMostSignificantBits(F, ceil(-log2(targ->probs[targ->i])) + 1.0);
}

// this function will count the number of occurrences of character c in
// char array of symbols
int count(char c, char symbols[], int n)
{
    int total = 0;
    for (int i = 0; i < n; i++)
    {

        if (symbols[i] == c)
            total++;
    }
    return total;
}

// this function return the array of unique symbols only from symbols array
char *charactersInSymbols(char symbols[], int n)
{
    char *c = (char *)calloc(7, sizeof(char));
    // flags to check if symbol is already seen or not
    bool aFlag = false;
    bool bFlag = false;
    bool cFlag = false;
    bool dFlag = false;
    bool eFlag = false;
    bool fFlag = false;

    int i2 = 0;
    for (int i = 0; i < n; i++)
    {
        if (symbols[i] == 'A' && !aFlag)
        {
            c[i2] = 'A';
            i2++;
            aFlag = true;
        }
        else if (symbols[i] == 'B' && !bFlag)
        {
            c[i2] = 'B';
            i2++;
            bFlag = true;
        }
        else if (symbols[i] == 'C' && !cFlag)
        {
            c[i2] = 'C';
            i2++;
            cFlag = true;
        }
        else if (symbols[i] == 'D' && !dFlag)
        {
            c[i2] = 'D';
            i2++;
            dFlag = true;
        }
        else if (symbols[i] == 'E' && !eFlag)
        {
            c[i2] = 'E';
            i2++;
            eFlag = true;
        }
        else if (symbols[i] == 'F' && !fFlag)
        {
            c[i2] = 'F';
            i2++;
            fFlag = true;
        }
        else if (symbols[i] == '1' && !bFlag)
        {
            c[i2] = '1';
            i2++;
            bFlag = true;
        }
        else if (symbols[i] == '2' && !cFlag)
        {
            c[i2] = '2';
            i2++;
            cFlag = true;
        }
        else if (symbols[i] == '3' && !dFlag)
        {
            c[i2] = '3';
            i2++;
            dFlag = true;
        }
        else if (symbols[i] == '4' && !eFlag)
        {
            c[i2] = '4';
            i2++;
            eFlag = true;
        }
        else if (symbols[i] == '5' && !fFlag)
        {
            c[i2] = '5';
            i2++;
            fFlag = true;
        }
    }

    c[i2] = '\0';
    return c;
}

int main(void)
{
    // read symbols from input
    char symbols[1024];
    scanf("%[^\n]s", symbols);
    size_t const n = strlen(symbols);

    // to keep record of unique characters
    char *characters = (char *)calloc(7, sizeof(char));
    characters = charactersInSymbols(symbols, n); // extracting unique characters from symbols
    size_t const n2 = strlen(characters);
    double *probs = (double *)calloc(n2, sizeof(double));
    pthread_t *tids = (pthread_t *)calloc(n2, sizeof(pthread_t));
    ThreadArg *targs = (ThreadArg *)calloc(n2, sizeof(ThreadArg));

    for (long i = 0; i < n2; ++i)
    {
        if (pthread_mutex_init(&((targs + i)->lock), NULL) != 0)
        {
            // intializing the lock
            printf("\n mutex init has failed\n");
            return 1;
        }
        probs[i] = (double)(count(characters[i], symbols, n) / (double)n); // calculating probability of character based on its occurence count
        targs[i] = (ThreadArg){(size_t)i, probs, (targs + i)->lock};
        pthread_create(tids + i, NULL, encode, (void *)(targs + i)); // creating thread
    }
    // display output
    puts("SHANNON-FANO-ELIAS Codes:\n");
    for (size_t i = 0; i < n2; ++i)
    {
        ThreadReturn *code;
        pthread_join(tids[i], (void **)&code);
        printf("Symbol %c, Code: ", characters[i]);
        for (size_t i = code->n; i > 0; --i)
        {
            printf("%ld", (code->bits >> i) & 1);
        }
        free(code);
        puts("");
    }

    // free used memory
    free(targs);
    free(tids);
    free(probs);
}