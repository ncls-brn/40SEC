#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>

#define SHM_NAME_INBOX "/inbox_shm"
#define SHM_NAME_OUTBOX "/outbox_shm"
#define SEM_NAME_INBOX "/inbox_sem"
#define SEM_NAME_OUTBOX "/outbox_sem"

pid_t other_pid = 0;
char *inbox = NULL;
char *outbox = NULL;
sem_t *sem_inbox = NULL;
sem_t *sem_outbox = NULL;

void cleanup() {
    // Cleanup the shared memory and semaphores
    if (inbox != NULL) {
        munmap(inbox, 1024);
        shm_unlink(SHM_NAME_INBOX);
    }
    if (outbox != NULL) {
        munmap(outbox, 1024);
        shm_unlink(SHM_NAME_OUTBOX);
    }
    if (sem_inbox != NULL) {
        sem_close(sem_inbox);
        sem_unlink(SEM_NAME_INBOX);
    }
    if (sem_outbox != NULL) {
        sem_close(sem_outbox);
        sem_unlink(SEM_NAME_OUTBOX);
    }
    exit(EXIT_SUCCESS);
}

void sigint_handler(int sig) {
    printf("Received SIGINT. Sending SIGTERM to other process.\n");
    kill(other_pid, SIGTERM); // Send SIGTERM to other process
    cleanup();
}

void sigterm_handler(int sig) {
    printf("Received SIGTERM. Cleaning up and exiting.\n");
    cleanup();
}

void sigusr1_handler(int sig) {
    // Display the inbox content immediately
    fputs("Received message: ", stdout);
    fputs(inbox, stdout);
    fflush(stdout); // Ensure the message is displayed immediately
    inbox[0] = '\0'; // Clear the inbox so the other process knows it can send more messages
}

void wait_for_outbox_to_be_empty() {
    // Wait for the other process to finish reading the message in the outbox
    sem_wait(sem_outbox);
}

void read_and_write_messages() {
    char *line = NULL;
    size_t line_length = 0;

    // Main loop: Read lines from the user and send to outbox
    while (1) {
        printf("Enter message: ");
        
        // Read a line of input from the user
        ssize_t bytes_read = getline(&line, &line_length, stdin);
        if (bytes_read == -1) {
            // If we reach EOF, break out of the loop
            printf("EOF reached. Sending SIGTERM to other process.\n");
            kill(other_pid, SIGTERM);  // Send SIGTERM to the other process
            break;
        }

        // Remove the newline character at the end of the line (if present)
        if (line[bytes_read - 1] == '\n') {
            line[bytes_read - 1] = '\0';
        }

        // Wait for the outbox to be empty (message read by the other process)
        wait_for_outbox_to_be_empty();

        // Write the message to the outbox (shared memory)
        strncpy(outbox, line, 1024);  // Ensure no overflow by limiting the size

        // Signal the other process that there's a new message in the outbox
        sem_post(sem_inbox);

        // Wait for the other process to read the outbox (check inbox)
        sem_wait(sem_outbox);
    }

    free(line);  // Clean up the allocated memory for user input line
}

int main(void) {
    // Print out this process's PID
    printf("This process's ID: %ld\n", (long) getpid());

    char *line = NULL;
    size_t line_length = 0;

    // Prompt for the other process's PID
    do {
        printf("Enter other process ID: ");
        if (-1 == getline(&line, &line_length, stdin)) {
            perror("getline");
            abort();
        }
    } while ((other_pid = strtol(line, NULL, 10)) == 0);
    free(line);

    // Create shared memory regions
    int shm_inbox_fd = shm_open(SHM_NAME_INBOX, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shm_inbox_fd, 1024);
    inbox = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_inbox_fd, 0);
    close(shm_inbox_fd);

    int shm_outbox_fd = shm_open(SHM_NAME_OUTBOX, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shm_outbox_fd, 1024);
    outbox = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_outbox_fd, 0);
    close(shm_outbox_fd);

    // Create semaphores for synchronization
    sem_inbox = sem_open(SEM_NAME_INBOX, O_CREAT, S_IRUSR | S_IWUSR, 0); // inbox starts empty
    sem_outbox = sem_open(SEM_NAME_OUTBOX, O_CREAT, S_IRUSR | S_IWUSR, 1); // outbox starts ready for writing

    // Set up signal handlers
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);
    signal(SIGUSR1, sigusr1_handler);

    // Start the read and write messages loop
    read_and_write_messages();

    // Perform cleanup
    cleanup();

    return EXIT_SUCCESS;
}
