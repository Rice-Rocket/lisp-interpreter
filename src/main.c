#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main() {
    puts("Lisp Version 0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char *input = readline("Lisp > ");
        add_history(input);

        printf("%s\n", input);

        free(input);
    }

    return 0;
}
