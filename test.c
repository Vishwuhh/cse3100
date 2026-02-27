void run(int a[], int n) {
    printf("%ld\n", sizeof((int)a[1]));
}

int main() {
    int a[10] = {1, 2, 3};
    run(a, 10);
    printf("%ld\n", sizeof(a));
}
