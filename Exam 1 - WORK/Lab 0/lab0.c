#include <stdio.h>

int main(void)
{
    int num = 2;
    int sum = 0;

    while (num < 200) {
        sum += num;
        num += 2;
    }

    printf("Hello, World!\n");
    printf("%i\n", sum);
    return 0;
}
