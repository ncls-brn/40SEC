#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>

pthread_barrier_t barrier; // pour synchroniser les philosophes

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void *eat(void *arg) {
    int n = (int) (long)arg;

    
    pthread_barrier_wait(&barrier);

    
    int left = n;
    int right = (n + 1) % 5;

    if (left > right) {  
        left = (n + 1) % 5;
        right = n;
    }

    // Prend la plus petite
    pthread_mutex_lock(&chopstick[left]);
    printf("Philosopher %d got chopstick left %d\n", n, left);
    
    // Prend la plus grande
    pthread_mutex_lock(&chopstick[right]);
    printf("Philosopher %d got chopstick right %d\n", n, right);
    
    printf("Philosopher %d is eating\n", n);
    sleep(1);
    
    // Pose
    printf("Philosopher %d set down chopstick right %d\n", n, right);
    pthread_mutex_unlock(&chopstick[right]);
    printf("Philosopher %d set down chopstick left %d\n", n, left);
    pthread_mutex_unlock(&chopstick[left]);

    return NULL;
}

int main(int argc, const char *argv[]) {
    pthread_barrier_init(&barrier, NULL, 5);

    
    for(int i = 0; i < 5; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);

   
    for(int i = 0; i < 5; i += 1)
        pthread_create(&philosopher[i], NULL, eat, (void *)(size_t)i);

    
    for(int i = 0; i < 5; i += 1)
        pthread_join(philosopher[i], NULL);

    
    for(int i = 0; i < 5; i += 1)
        pthread_mutex_destroy(&chopstick[i]);

    pthread_barrier_destroy(&barrier);

    return 0;
}
