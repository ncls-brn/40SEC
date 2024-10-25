#include <stdio.h>

int main(int argc, char *argv[]) {
    // Vérifier si un argument a été passé
    if (argc > 1) {
        // Afficher "Hello" suivi du premier argument
        printf("Hello, %s!\n", argv[1]);
    } else {
        // Si aucun argument n'est passé, afficher un message d'erreur
        printf("Usage: %s <name>\n", argv[0]);
    }

    return 0;
}


