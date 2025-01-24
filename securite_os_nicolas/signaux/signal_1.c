#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define SHM_SIZE 4096 // Taille de la mémoire partagée (en octets)

pid_t other_pid = 0; // PID du processus de l'autre utilisateur

// Nom du fichier de la boîte de réception
char inbox_filename[512];

// Pointeur vers la mémoire partagée
char *inbox_data;

int create_shared_memory(const char *filename) {
    // Ouvrir la mémoire partagée
    int inbox_fd = shm_open(filename, O_CREAT | O_RDWR, 0666);
    if (inbox_fd < 0) {
        perror("shm_open failed");
        return -1;
    }

    // Définir la taille de la mémoire partagée
    if (ftruncate(inbox_fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        close(inbox_fd);
        return -1;
    }

    // Mapper la mémoire partagée
    inbox_data = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
    if (inbox_data == MAP_FAILED) {
        perror("mmap failed");
        close(inbox_fd);
        return -1;
    }

    // Fermer le descripteur de fichier, la mémoire reste accessible
    close(inbox_fd);

    return 0;
}
void cleanup_shared_memory(const char *filename) {
    if (inbox_data != MAP_FAILED) {
        munmap(inbox_data, SHM_SIZE); // Détacher la mémoire partagée
    }
    shm_unlink(filename); // Supprimer la mémoire partagée
}
int main(void) {
    pid_t my_pid = getpid(); // Récupérer l'ID du processus
    printf("This process's ID: %ld\n", (long) my_pid);

    // Créer un nom unique pour la boîte de réception de ce processus
    snprintf(inbox_filename, sizeof(inbox_filename), "/%d-chat", my_pid);

    // Créer et mapper la mémoire partagée pour la boîte de réception de ce processus
    if (create_shared_memory(inbox_filename) < 0) {
        return EXIT_FAILURE;
    }

    // Créer un nom pour la boîte de réception de l'autre processus (ID de l'autre processus à obtenir de l'utilisateur)
    printf("Enter other process ID: ");
    scanf("%d", &other_pid);
    
    char other_inbox_filename[512];
    snprintf(other_inbox_filename, sizeof(other_inbox_filename), "/%d-chat", other_pid);

    // Créer et mapper la mémoire partagée pour la boîte de réception de l'autre processus
    if (create_shared_memory(other_inbox_filename) < 0) {
        return EXIT_FAILURE;
    }

    // Exemple de communication : écrire un message dans la boîte de réception de l'autre processus
    snprintf(inbox_data, SHM_SIZE, "Hello from process %ld", (long) my_pid);

    // Attendre que l'autre processus lise le message
    printf("Waiting for message from process %ld...\n", (long) other_pid);
    while (strlen(inbox_data) == 0) {
        sleep(1); // Attente active (simulée ici)
    }

    // Afficher le message reçu de l'autre processus
    printf("Message from process %ld: %s\n", (long) other_pid, inbox_data);

    // Nettoyer la mémoire partagée
    cleanup_shared_memory(inbox_filename);
    cleanup_shared_memory(other_inbox_filename);

    return EXIT_SUCCESS;
}
