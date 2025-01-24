#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define SHM_SIZE 4096 // Taille de la mémoire partagée (en octets)

pid_t other_pid = 0; // PID du processus de l'autre utilisateur
char *inbox_data; // Pointeur vers la boîte de réception
char *outbox_data; // Pointeur vers la boîte d'envoi
char inbox_filename[512]; // Nom du fichier de la boîte de réception
char outbox_filename[512]; // Nom du fichier de la boîte d'envoi

// Fonction pour créer et mapper la mémoire partagée
int create_shared_memory(const char *filename) {
    int inbox_fd = shm_open(filename, O_CREAT | O_RDWR, 0666);
    if (inbox_fd < 0) {
        perror("shm_open failed");
        return -1;
    }

    if (ftruncate(inbox_fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        close(inbox_fd);
        return -1;
    }

    // Mapper la mémoire partagée pour inbox_data
    inbox_data = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
    if (inbox_data == MAP_FAILED) {
        perror("mmap failed for inbox_data");
        close(inbox_fd);
        return -1;
    }

    // Mapper la mémoire partagée pour outbox_data
    outbox_data = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
    if (outbox_data == MAP_FAILED) {
        perror("mmap failed for outbox_data");
        munmap(inbox_data, SHM_SIZE); // Nettoyage
        close(inbox_fd);
        return -1;
    }

    close(inbox_fd);
    return 0;
}

// Fonction de nettoyage de la mémoire partagée
void cleanup_shared_memory(const char *filename) {
    if (inbox_data != MAP_FAILED) {
        munmap(inbox_data, SHM_SIZE);
    }
    if (outbox_data != MAP_FAILED) {
        munmap(outbox_data, SHM_SIZE);
    }
    shm_unlink(filename);
}

// Gestionnaire de SIGUSR1 : lire le message et signaler la fin
static void handle_sigusr1(int signum) {
    if (signum == SIGUSR1) {
        fputs(inbox_data, stdout); // Afficher le message
        fflush(stdout); // Assurer l'affichage immédiat
        inbox_data[0] = '\0'; // Réinitialiser la boîte de réception pour signaler la fin
        kill(other_pid, SIGUSR2); // Envoyer SIGUSR2 pour signaler que le message a été traité
    }
}

// Fonction de signalisation que B est prêt
void ready_to_receive() {
    outbox_data[0] = 'r';  // Marque que B est prêt à recevoir
    kill(other_pid, SIGUSR2); // Envoie SIGUSR2 à A
}

// Fonction principale
int main(void) {
    pid_t my_pid = getpid();
    printf("This process's ID: %ld\n", (long) my_pid);

    // Créer un nom unique pour la boîte de réception et la boîte d'envoi
    snprintf(inbox_filename, sizeof(inbox_filename), "/%d-chat-inbox", my_pid);
    snprintf(outbox_filename, sizeof(outbox_filename), "/%d-chat-outbox", my_pid);

    if (create_shared_memory(inbox_filename) < 0) {
        return EXIT_FAILURE;
    }

    // Demander à l'utilisateur de saisir l'ID de l'autre processus
    printf("Enter other process ID: ");
    
    // Bloquer SIGUSR1 et SIGUSR2 pendant la saisie du PID
    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    sigaddset(&block_set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &block_set, NULL);

    // Lire l'ID du processus de l'autre utilisateur
    scanf("%d", &other_pid);

    // Débloquer les signaux après la saisie
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);

    char other_inbox_filename[512];
    snprintf(other_inbox_filename, sizeof(other_inbox_filename), "/%d-chat-inbox", other_pid);

    // Créer et mapper la mémoire partagée pour la boîte de réception de l'autre processus
    if (create_shared_memory(other_inbox_filename) < 0) {
        return EXIT_FAILURE;
    }

    // Configurer le gestionnaire pour SIGUSR1
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    // Signaler que B est prêt à recevoir un message
    ready_to_receive();

    // Synchronisation pour attendre que le processus B soit prêt
    printf("Waiting for process %d to be ready...\n", other_pid);
    while (outbox_data[0] == '\0') {
        // Attente active : attend que le processus B envoie un signal ou modifie `outbox_data`
        sleep(1);
    }

    // Envoyer un message à l'autre processus
    while (1) {
        printf("Enter a message to send: ");
        fgets(inbox_data, SHM_SIZE, stdin);

        // Envoyer SIGUSR1 à l'autre processus pour signaler qu'un message est prêt
        kill(other_pid, SIGUSR1);

        // Attendre que l'autre processus lise le message (attente du '\0')
        while (outbox_data[0] != '\0') {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 };
            nanosleep(&ts, NULL);
        }

        // Réinitialiser la boîte de réception avant d'envoyer un autre message
        inbox_data[0] = '\0';
    }

    cleanup_shared_memory(inbox_filename);
    cleanup_shared_memory(other_inbox_filename);

    return EXIT_SUCCESS;
}
