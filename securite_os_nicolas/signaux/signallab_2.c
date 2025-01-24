#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// Taille de la mémoire partagée
#define SHARED_MEMORY_SIZE 4096

pid_t other_pid = 0;
char *inbox_data;
char *outbox_data;


sigset_t my_sigset;

// Configuration des gestionnaires de signaux
static void handle_signal(int signum) {
    if (signum == SIGTERM) {
        printf("Received SIGTERM. Cleaning up...\n");
        cleanup_shared_memory();
        exit(EXIT_SUCCESS);
    } else if (signum == SIGINT) {
        printf("Received SIGINT. Cleaning up and sending SIGTERM to other process...\n");
        cleanup_shared_memory();
        kill(other_pid, SIGTERM);
        exit(EXIT_SUCCESS);
    } else if (signum == SIGUSR1) {
        printf("Received SIGUSR1. Displaying inbox:\n");
        printf("%s\n", inbox_data);
        inbox_data[0] = '\0';  
        fflush(stdout);

        
        printf("Sending SIGUSR2 to the other process\n");
        kill(other_pid, SIGUSR2);
    }
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    // Bloquer SIGUSR2 pour utiliser sigwait
    printf("Blocking SIGUSR2 in the signal mask\n");
    sigemptyset(&my_sigset);
    sigaddset(&my_sigset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &my_sigset, NULL);
    printf("SIGUSR2 has been blocked\n");
}

// Fonction de nettoyage de la mémoire partagée
void cleanup_shared_memory() {
    if (inbox_data != NULL) {
        munmap(inbox_data, SHARED_MEMORY_SIZE);
    }
    if (outbox_data != NULL) {
        munmap(outbox_data, SHARED_MEMORY_SIZE);
    }

    // Supprimer les régions de mémoire partagée
    char inbox_filename[512];
    snprintf(inbox_filename, sizeof(inbox_filename), "/%d-chat", getpid());
    shm_unlink(inbox_filename);

    char outbox_filename[512];
    snprintf(outbox_filename, sizeof(outbox_filename), "/%d-chat", other_pid);
    shm_unlink(outbox_filename);
}

// Fonction pour créer la mémoire partagée pour la boîte de réception et la boîte d'envoi
void setup_shared_memory(pid_t other_pid) {
    char inbox_filename[512];
    snprintf(inbox_filename, sizeof(inbox_filename), "/%d-chat", getpid());

    int inbox_fd = shm_open(inbox_filename, O_CREAT | O_RDWR, 0666);
    if (inbox_fd < 0) {
        perror("Failed to open inbox shared memory");
        exit(EXIT_FAILURE);
    }

    ftruncate(inbox_fd, SHARED_MEMORY_SIZE);
    inbox_data = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
    if (inbox_data == MAP_FAILED) {
        perror("Failed to map inbox shared memory");
        exit(EXIT_FAILURE);
    }
    close(inbox_fd);

    // Configurer la boîte d'envoi (la boîte de réception du processus autre)
    char outbox_filename[512];
    snprintf(outbox_filename, sizeof(outbox_filename), "/%d-chat", other_pid);

    int outbox_fd = shm_open(outbox_filename, O_CREAT | O_RDWR, 0666);
    if (outbox_fd < 0) {
        perror("Failed to open outbox shared memory");
        exit(EXIT_FAILURE);
    }

    ftruncate(outbox_fd, SHARED_MEMORY_SIZE);
    outbox_data = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, outbox_fd, 0);
    if (outbox_data == MAP_FAILED) {
        perror("Failed to map outbox shared memory");
        exit(EXIT_FAILURE);
    }
    close(outbox_fd);
}

// Fonction pour envoyer des messages à la boîte d'envoi
void send_to_outbox() {
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), stdin)) {
        // Copier les données dans la boîte d'envoi
        strncpy(outbox_data, buffer, SHARED_MEMORY_SIZE);
        outbox_data[SHARED_MEMORY_SIZE - 1] = '\0';  

        // Envoyer SIGUSR1 au processus autre pour lui notifier que des données sont prêtes
        printf("Sending SIGUSR1 to the other process\n");
        kill(other_pid, SIGUSR1);

        // Attendre que la boîte d'envoi soit vide

        // Attendre le signal SIGUSR2 pour continuer
        printf("Waiting for SIGUSR2 to continue\n");
        int sig_received;
        int ret = sigwait(&my_sigset, &sig_received);
        if (ret != 0) {
            perror("sigwait failed");
            exit(EXIT_FAILURE);
        }
        printf("Received SIGUSR2 from the other process\n");
    }
}

// Fonction principale
int main(void) {
    printf("This process's ID: %ld\n", (long) getpid());

    // Configurer les gestionnaires de signaux
    setup_signal_handlers();

    
    char *line = NULL;
    size_t line_length = 0;
    do {
        printf("Enter other process ID: ");
        if (-1 == getline(&line, &line_length, stdin)) {
            perror("getline");
            abort();
        }
    } while ((other_pid = strtol(line, NULL, 10)) == 0);
    free(line);

   
    setup_shared_memory(other_pid);

    
    send_to_outbox();

    
    cleanup_shared_memory();

    return EXIT_SUCCESS;
}
