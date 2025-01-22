#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_BACKOFF 400
#define MIN_BACKOFF 50  

pthread_barrier_t barrier;

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void sleep_randomized(int attempt) {
    
    int wait_time = (rand() % (MIN_BACKOFF * (1 << attempt))) + MIN_BACKOFF;
    usleep(wait_time);
}

void *eat(void *arg) {
    int n = (int) (long)arg;
    int attempt = 0;

    pthread_barrier_wait(&barrier);

    while (1) {
        
        if (pthread_mutex_trylock(&chopstick[n]) == 0) {
            printf("Philosopher %d got chopstick %d\n", n, n);

            if (pthread_mutex_trylock(&chopstick[(n + 1) % 5]) == 0) {
                printf("Philosopher %d got chopstick %d\n", n, (n + 1) % 5);

               
                printf("Philosopher %d is eating\n", n);
                sleep(1);
                printf("Philosopher %d is now sleeping ...\n", n);

                
                printf("Philosopher %d set down chopstick %d\n", n, (n + 1) % 5);
                pthread_mutex_unlock(&chopstick[(n + 1) % 5]);
                printf("Philosopher %d set down chopstick %d\n", n, n);
                pthread_mutex_unlock(&chopstick[n]);
                break; 
            } else {
                
                pthread_mutex_unlock(&chopstick[n]);
                sleep_randomized(attempt);  
                attempt++;
            }
        } else {
            
            sleep_randomized(attempt);  
            attempt++;
        }
    }
    
    return NULL;
    
}

int main(int argc, const char *argv[]) {
    srand(time(NULL)); 

    pthread_barrier_init(&barrier, NULL, 5);

    
    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(&chopstick[i], NULL);
    }

    
    for (int i = 0; i < 5; i++) {
        pthread_create(&philosopher[i], NULL, eat, (void *)(size_t)i);
    }

    
    for (int i = 0; i < 5; i++) {
        pthread_join(philosopher[i], NULL);
    }

    
    for (int i = 0; i < 5; i++) {
        pthread_mutex_destroy(&chopstick[i]);
    }

    pthread_barrier_destroy(&barrier);

    printf("All philosophers are eating and now are sleeping\n");

    return 0;
}
