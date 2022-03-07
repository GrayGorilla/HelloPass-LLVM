void test() {
    int a, b, c, e;
    a = b = 5;
    c = 0;
    e = 4;

    for (int i = 0; i < 100; i++) {
        c = a - b;
        while (i + 3 < 20) {
            i = a + i;
        }
        b = c * a;
    }
}
