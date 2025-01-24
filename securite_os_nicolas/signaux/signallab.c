#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// Taille de la mémoire partagée
#define SHARED_MEMORY_SIZE 4096

pid_t other_pid = 0;
char *inbox_data;
char *outbox_data;

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
        // Afficher le contenu de la boîte de réception et réinitialiser
        printf("Received SIGUSR1. Displaying inbox:\n");
        printf("%s\n", inbox_data);
        inbox_data[0] = '\0';  // Effacer la boîte de réception
        fflush(stdout);
        //  ajouter kill  other_pid siguser2 
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
}

// Fonction de nettoyage de la mémoire partagée
void cleanup_shared_memory() {
    if (inbox_data != NULL) {
        munmap(inbox_data, SHARED_MEMORY_SIZE);
    }
    if (outbox_data != NULL) {
        munmap(outbox_data, SHARED_MEMORY_SIZE);
    }

    
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
        kill(other_pid, SIGUSR1);

        // Attendre que la boîte d'envoi soit vide (c'est-à-dire que l'autre processus ait consommé les données)
        while (outbox_data[0] != '\0') {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; // 10ms
            nanosleep(&ts, NULL);
        }
        //sigwait attent avant siguser2 
    }
}

// Fonction principale
int main(void) {
    printf("This process's ID: %ld\n", (long) getpid());

    
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

    // Configurer la mémoire partagée
    setup_shared_memory(other_pid);

    
    send_to_outbox();

    
    cleanup_shared_memory();

    return EXIT_SUCCESS;
}
