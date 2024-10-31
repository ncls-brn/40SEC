#include <stdio.h>

int main(int argc, char *argv[]) {
    
    if (argc > 1) {
      
        printf("Hello, %s!\n", argv[1]);
    } else {
        
        printf("Usage: %s <name>\n", argv[0]);
    }

    return 0;
}


