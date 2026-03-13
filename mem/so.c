#include <stdio.h>

void triggerStackOverflow(int count) {
    printf("Call number: %d\n", count);
    triggerStackOverflow(count + 1);
}

int main() {
    triggerStackOverflow(1);
    return 0;
}