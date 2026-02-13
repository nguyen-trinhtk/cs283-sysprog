#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PIPE_STRING "|"

int main() {
    char input[] = "nani | bar | baz";
    char *segment = strtok(input, PIPE_STRING);
    int i = 0;
    while (segment != NULL) {
        // Trim leading whitespace
        while (*segment && isspace(*segment)) segment++;
        // Trim trailing whitespace
        char *end = segment + strlen(segment) - 1;
        while (end > segment && isspace(*end)) {
            *end = '\0';
            end--;
        }
        printf("Segment %d: '%s'\n", i, segment);
        i++;
        segment = strtok(NULL, PIPE_STRING);
    }
    return 0;
}
